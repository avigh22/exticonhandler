var objs = [{name:"a",id:"bc"},{name:"b",id:"12"}]
var inst = {name:"a",id:"bc"}

function sprintf_type(t){
	switch (typeof t ){
	case "number": 
		return t.toString();
	case "string":
		return '"' + t + '"';
	case "boolean":
		return t.toString();
	case "object":
		return sprintf_obj(t);
	case "function":
		return t.toString();
	case "undefined":
		return "undefined";
	}
}

function sprintf_obj(obj){	
	if(typeof obj == "object"){
		if(Object.prototype.toString.call(obj) == "[object Array]"){
			var objString = '[';
			for(i in obj){
				objString +=sprintf_type( obj[i]);
				objString +=",";
			}
			if(objString.length>2)
				objString = objString.substr(0, objString.length-1);
			objString += "]";
			return objString;
			
		}else if(Object.prototype.toString.call(obj) == "[object Object]"){
			var objString = "{";
			for(i in obj){
				objString += sprintf_type(i);
				objString += ":";
				objString += sprintf_type(obj[i]);
				objString += ",";
			}
			if(objString.length>2)
				objString = objString.substr(0, objString.length-1);
			objString += "}";
			return objString;
		}
	}	
}

var arrayStr=["1", inst, WScript, WScript];  
//WScript.Echo(Object.prototype.toString.call (sprintf_obj))
//WScript.Echo(Object.prototype.toString.call(arrayStr))
//WScript.Echo(arrayStr instanceof Array ===true); 
//WScript.Echo(arrayStr instanceof Object ===true); 

  
var ttt=true
//WScript.Echo(arrayStr.valueOf());
WScript.Echo(sprintf_obj(inst) )
WScript.Echo(sprintf_obj(objs) )
WScript.Echo(sprintf_obj(arrayStr) )
 WScript.Echo(WScript instanceof Object)
 WScript.Echo(WScript)

//WScript.Echo(sprintf_light(inst))