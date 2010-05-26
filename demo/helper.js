
function Rpc(url, service) {
	this.url = url;
	this.service = service;
	this.id = 0;
	this.callAsync = function(method, data, dataFunc, errFunc) {
		this.id = this.id+1;
		var d =  {
			"service": this.service,
			"method": method,
			"id": this.id,
			"params": data
		};
		var ds = JSON.stringify(d);
		$.post(this.url, ds, function(data) {
			if (data.error == null)
				dataFunc(data.id, data.result);
			else if (errFunc)
				errFunc(data.id, data.error);
		}, "json");
		return this.id;
	}
}

