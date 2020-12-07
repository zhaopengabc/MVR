
importScripts('/static/video/libffmpeg.js');

var cxt = null
var start = false
db = null
function openDB (name,version) {
            var version=version || 1;
            var request=indexedDB.open(name,version);
            request.onerror=function(e){
                console.log(e.currentTarget.error.message);
				openDB(name,version)
			}
			request.onsuccess=function(e){
                db = e.target.result;
            };
}
var dbUpdateMutex = 0
var frameObjList = []
function updateData(db,storeName,objStore){
			frameObjList.push(objStore)
			if(frameObjList.length < 1)
				return
			dbUpdateMutex++
            var transaction=db.transaction(storeName,'readwrite');
            var store=transaction.objectStore(storeName);
			var request = store.add(frameObjList);
			frameObjList = []
            request.onsuccess=function(e){ 
					dbUpdateMutex--
            };
			request.onerror = function(e){
					dbUpdateMutex--
			}
		/****
            var request=store.get(objStore.id); 
            request.onsuccess=function(e){ 
                var frameObj=e.target.result; 
				if(frameObj.data === null)
				{
					var req = store.put(objStore);
					req.onsuccess = function(e)
					{
							dbUpdateMutex--
					}
					req.onerror = function(e)
					{
							dbUpdateMutex--
					}
				}
				else
				{
					var req = store.put(objStore);
					req.onsuccess = function(e)
					{
							dbUpdateMutex--
					}
						//dbUpdateMutex = false
				}
                //console.log(frameObj); 
            };
			request.onerror = function(e){
					dbUpdateMutex--
			}
			***/
}
//openDB('frameDB',2)
function waitWasmLoading()
{
    console.log("timer ...")
    //if(Module['asm']['_zx_createH264Decoder'] != undefined)
    //if(Module['asm']['O'] != undefined)
    //console.log(Module._zx_createH264Decoder)
    if('_zx_createH264Decoder' in Module.asm)
    {

        console.log("timer stop")
        cxt = Module._zx_createH264Decoder()
        clearInterval(timerWasm)
        setTimeout("start = true;",1000)
        postMessage({type:1})
        //startWork()
    }
}
var timerWasm = setInterval(waitWasmLoading,100)
function copyData2(ptr,len)
{
    return Module.HEAPU8.slice(ptr,ptr+len)
}
function copyData(ptr,len)
{

    var buff = new ArrayBuffer(len)
    var u8Buff = new Uint8Array(buff)
    for(var i = 0;i < len;i++)
        u8Buff[i] = Module._zx_getMemVal(ptr,i)
    return u8Buff
}

onmessage = function (event){
    if(event.data.type == 1) //recv h264
    {
        if(start == false)
            return
        var h264Buff = event.data.h264Buff
        var buffer = Module._malloc(h264Buff.length)
        Module.HEAPU8.set(h264Buff, buffer);
        Module._zx_decode(cxt,buffer,h264Buff.length)
	Module._free(buffer)
	
        var frame =_zx_getFrame(cxt);
        while(frame != 0)
        {
			var len = Module._zx_getMemVal(frame)
			len += Module._zx_getMemVal(frame+1)<<8
			len += Module._zx_getMemVal(frame+2)<<16
			len += Module._zx_getMemVal(frame+3)<<24
            frameBuff = copyData2(frame,len+4)
            Module._free(frame)
			if(dbUpdateMutex < 3 )
			{
				//updateData(db,'frameObj',{'id':0,data:frameBuff})
				var data = {type:2,frame:frameBuff}
				//postMessage(data,[data.frame.buffer])
				postMessage(data)
			}
			//console.log("post frame...")
            frame =_zx_getFrame(cxt);
        }
    }

}

