$(document).ready(function(){
	var rName = getRootName();
	setHeadTitle("Sequence Viewer");
	addDiv("body", "viewer");
	var gifLoading = prsentDivGifLoading();
	getJSON('/' + rName + '/seqData').then(function (mainData) {
		var sesUid = mainData["sessionUID"];
		var SeqViewer = new njhSeqView("#viewer", mainData);
		gifLoading.remove();
		setUpCloseSession(sesUid);
	}).catch(function(err){
  		removeAllDivGifLoading();
  		logRequestError(err);
  	});
});
