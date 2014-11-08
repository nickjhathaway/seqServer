// based on example from http://thecodeplayer.com/walkthrough/html5-game-tutorial-make-a-snake-game-using-html5-canvas-jquery

$(document).ready(function(){
	function addMouseScrollListener(obj, up, down){
        // from http://www.sitepoint.com/html5-javascript-mouse-wheel/
        this.handler = function(e){
        
	    var e = window.event || e; // old IE support
	    
	    var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
            if( delta > 0){
                up(delta);
            } else {
                down(delta);
            }
         e.preventDefault();
        };
        if (obj.addEventListener) {
	    // IE9, Chrome, Safari, Opera
	    obj.addEventListener("mousewheel", handler, false);
	    // Firefox
	    obj.addEventListener("DOMMouseScroll", handler, false);
        } else {
            // IE 6/7/8
            obj.attachEvent("onmousewheel", handler);
        }
    }
    
    var getRelCursorPosition = function(event, obj ) {
        // from http://stackoverflow.com/a/5417934
        var canoffset = $(obj).offset();
        var x = event.clientX + document.body.scrollLeft +
            document.documentElement.scrollLeft - Math.floor(canoffset.left);
        var y = event.clientY + document.body.scrollTop +
            document.documentElement.scrollTop - Math.floor(canoffset.top) + 1;
        return [x,y];
    };
    
	function Canvas(i){
        this.canvas = $(i)[0];
        this.context = this.canvas.getContext("2d");
    }
	
	function SeqPainter(cellWidth, cellHeight, numOfSeqs, numOfBases, nameOffSet, baseColors){
		this.needToPaint = true;
		this.cw = cellWidth;
		this.ch = cellHeight;
		this.nSeqs = numOfSeqs;
		this.nBases = numOfBases;
		this.bColors = baseColors;
		this.nameOffSet = nameOffSet;
	}
	
	//draw given seq
	SeqPainter.prototype.paintSeq = function(seqContext, index, seq, start){
    	seqContext.textAlign = "center";
    	seqContext.textBaseline = "middle";
    	seqContext.font = "bold 15px 'Helvetica Neue',Helvetica, Arial, sans-serif";
    	for(var wi = start; wi - start < this.nBases; wi++){
            if(wi < seq.length){
            	seqContext.fillStyle = this.bColors[seq[wi]];
            	seqContext.fillRect((wi - start) * this.cw + this.nameOffSet, index*this.ch, this.cw, this.ch);
            	seqContext.fillStyle = "#000000";
	        	seqContext.fillText(seq[wi], (wi -start)*this.cw + this.cw/2.0 + this.nameOffSet, index*this.ch + this.ch/2.0);
            }else{
            	seqContext.strokeStyle = "#000000";
    			seqContext.lineWidth   = 1;
            	seqContext.fillStyle = "#EEEEEE";
            	seqContext.fillRect((wi -start) * this.cw + this.nameOffSet, index*this.ch, this.cw, this.ch);
            	seqContext.strokeRect((wi -start) * this.cw + this.nameOffSet, index*this.ch, this.cw, this.ch);
            }
        }
	};
	
	//draw all necessary seqs
	SeqPainter.prototype.paintSeqs = function(seqContext, seqData, sStart, bStart){
    	if(this.needToPaint){
    		//console.log("painting");
    		//box around seqs
    		seqContext.strokeStyle = "#000000";
			seqContext.lineWidth   = 1;
	    	seqContext.strokeRect(this.nameOffSet, 0, this.cw * this.nBases, this.ch * this.nSeqs);
	    	//write names
	    	seqContext.textAlign = "left";
	    	seqContext.textBaseline = "middle";
	    	seqContext.font = "bold 15px 'Helvetica Neue',Helvetica, Arial, sans-serif";
	        seqContext.strokeStyle = "#000000";
			seqContext.lineWidth   = 1;
	    	for(var hi = sStart; hi -sStart < this.nSeqs ; hi++){
	    		seqContext.fillStyle = "#EEEEEE";
		    	seqContext.fillRect(0, (hi - sStart)*this.ch, this.nameOffSet, this.ch);
		    	seqContext.strokeRect(0, (hi - sStart)*this.ch, this.nameOffSet, this.ch);
		    	seqContext.fillStyle = "#000000";
				seqContext.fillText(seqData[hi]["name"], 2, (hi - sStart)*this.ch + this.ch/2.0);
	    	}	    	
	    	for(var hi = sStart; hi -sStart < this.nSeqs ; hi++){
	    		this.paintSeq(seqContext, hi - sStart,
	    		 seqData[hi]["seq"], bStart);
	    	}
	    	this.needToPaint = false;
	    	//console.log("end-painting");
    	}
	};
	//paint seq position
	SeqPainter.prototype.placeBasePos = function(seqContext, bStart){
    	seqContext.textAlign = "center";
    	seqContext.textBaseline = "middle";
    	seqContext.fillStyle = "#EEEEEE";
	    seqContext.fillRect(this.nameOffSet - this.cw, (this.nSeqs)*this.ch + 2 , this.cw * 2, this.ch);
	    seqContext.fillRect(this.nameOffSet - this.cw + this.nBases * this.cw -this.cw, (this.nSeqs)*this.ch + 2 , this.cw * 2, this.ch);
	    seqContext.fillStyle = "#000000";
    	seqContext.font = "bold 15px 'Helvetica Neue',Helvetica, Arial, sans-serif";
      	seqContext.fillText(bStart, this.nameOffSet, (this.nSeqs)*this.ch +2 + this.ch/2.0);
      	seqContext.fillText(bStart + this.nBases -1, nameOffSet + this.nBases * this.cw -this.cw, (this.nSeqs)*this.ch +2 + this.ch/2.0);
   };
	
	function SeqView(viewName, seqs, seqData, cellWidth, cellHeight, baseColors){
		//need to add style and html, currently just there
		//retrieve html elements 
		this.masterDiv = document.getElementById(viewName);
		this.canvas = $("#canvas", this.masterDiv)[0];
		this.context = this.canvas.getContext('2d');
		this.rSlider = $("#rightSlider", this.masterDiv)[0];
		this.bSlider = $("#bottomSlider", this.masterDiv)[0];
		this.popUp = $("#pop-up", this.masterDiv)[0];
		// set up of sizes
		$(this.masterDiv).width((window.innerWidth - 10) * 0.98);
		$(this.masterDiv).height((window.innerHeight - 60) * 0.98);
		this.canvas.width = $(this.masterDiv).width() * 0.98;
		this.canvas.height = $(this.masterDiv).height() * 0.95;
		var nameOffSet = 10 * cellWidth;
		var numOfBases = Math.floor((this.canvas.width - cellWidth - nameOffSet)/cellWidth);
	 	var numOfSeqs = Math.floor((this.canvas.height - cellHeight)/cellHeight);
		this.painter = new SeqPainter(cellWidth, cellHeight, numOfSeqs, numOfBases, nameOffSet, baseColors);
		this.seqs = seqs;
		this.seqData = seqData;
		this.seqStart = 0;
		this.baseStart = 0;
		this.currentSeq = 0;
		this.currentBase = 0;
		//
	};
	
	SeqView.prototype.setUp = function(){
		this.setUpCanvas();
		this.setUpSliders();
		this.setUpListeners();
	};
	
	SeqView.prototype.setUpCanvas = function(){
		$(this.masterDiv).width((window.innerWidth - 10) * 0.98);
		$(this.masterDiv).height((window.innerHeight - 60) * 0.98);
		this.canvas.width = $(this.masterDiv).width() * 0.98;
		this.canvas.height = $(this.masterDiv).height() * 0.95;
		this.painter.nBases = Math.floor((this.canvas.width - this.painter.cw - this.painter.nameOffSet)/this.painter.cw);
	 	this.painter.nSeqs = Math.floor((this.canvas.height - this.painter.ch)/this.painter.ch);
	};
	
	SeqView.prototype.updateCanvas = function(){
		var changingHeight = (window.innerHeight - 60) * 0.98;
		var changingWidth = (window.innerWidth - 10) * 0.98;
		$(this.masterDiv).width((window.innerWidth - 10) * 0.98);
		$(this.masterDiv).height((window.innerHeight - 60) * 0.98);
		this.canvas.width = $(this.masterDiv).width() * 0.98;
		this.canvas.height = $(this.masterDiv).height() * 0.95;
		if(changingHeight > this.canvas.height){
			this.painter.needToPaint = true;
		}
		if(changingWidth > this.canvas.width){
			this.painter.needToPaint = true;
		}
		this.painter.nBases = Math.floor((this.canvas.width - this.painter.cw - this.painter.nameOffSet)/this.painter.cw);
	 	this.painter.nSeqs = Math.floor((this.canvas.height - this.painter.ch)/this.painter.ch);
	};
	
	SeqView.prototype.paint = function(){
		this.painter.paintSeqs(this.context, this.seqs, this.seqStart, this.baseStart);
	};
	
    SeqView.prototype.mouseWheelUp = function(steps){
        if(this.seqStart > 0){
        	--this.seqStart;
        	$(this.rSlider).slider('value', this.seqData["numReads"] - this.seqStart - this.painter.nSeqs);
        	this.painter.needToPaint = true;
        	this.paint();
        }
    };

    SeqView.prototype.mouseWheelDown = function(steps){
        if(this.seqStart < Math.max(this.seqData["numReads"]- this.painter.nSeqs, 0)){
        	++this.seqStart;
        	$(this.rSlider).slider('value', this.seqData["numReads"] - this.seqStart - this.painter.nSeqs);
        	this.painter.needToPaint = true;
        	this.paint();
        }
    };
    
	SeqView.prototype.setUpSliders = function(){
    	$( this.bSlider ).css("left", this.painter.nameOffSet);
    	$( this.bSlider).css("width", this.painter.nBases * this.painter.cw);
    	$( this.rSlider ).css("height", this.painter.nSeqs * this.painter.ch);
	    $( this.bSlider).slider({
	      range: "min",
	      min: 0,
	      max: Math.max(this.seqData["maxLen"] - this.painter.nBases, 0),
	      value: 0,
	      slide :function(event, ui){
	      	this.baseStart = ui.value;
	      	this.painter.needToPaint = true;
	      	this.paint();
			this.painter.placeBasePos(context, baseStart);
	      }.bind(this)
	      }).bind(this);
	    $( this.rSlider ).slider({
	      range: "max",
	      min: 0,
	      max: Math.max(this.seqData["numReads"]- this.painter.nSeqs, 0),
	      value: this.seqData["numReads"],
	      orientation: "vertical", slide :function(event, ui){
	      	this.painter.needToPaint = true;
	      	this.seqStart = this.seqData["numReads"] - this.painter.nSeqs - ui.value;
	      	this.paint();
	      }.bind(this)
	    }).bind(this);
   };
   SeqView.prototype.clicked = function(e){
        var pt = getRelCursorPosition(e, this.canvas);
        this.currentBase = Math.ceil(pt[0]/this.painter.cw) - this.painter.nameOffSet/this.painter.cw + this.baseStart -1;
        this.currentSeq = Math.ceil(pt[1]/this.painter.ch) + this.seqStart - 1;
        console.log(pt);
        console.log(this.currentSeq);
        console.log(this.currentBase);
        console.log(this.seqs[this.currentSeq]["name"]);
        console.log(this.seqs[this.currentSeq]["seq"][this.currentBase]);
        console.log(this.seqs[this.currentSeq]["qual"][this.currentBase]);
    };
   SeqView.prototype.setUpListeners = function(){
   	// add scrolling listener
   	addMouseScrollListener(this.canvas, this.mouseWheelUp.bind(this), this.mouseWheelDown.bind(this));
   	this.canvas.addEventListener("mousedown", this.clicked.bind(this), false);
	//add hover box listening 
	var moveLeft = 20;
    var moveDown = 10;
    //object.hover(function(e) {
    $(this.canvas).hover(function(e) {
      //fadeInBox.fadeIn(500);
      $(this.popUp).fadeIn(500);
      //.css('top', e.pageY + moveDown)
      //.css('left', e.pageX + moveLeft)
      //.appendTo('body');
    }, function() {
      //fadeInBox.hide();
      $(this.popUp).hide();
    }).bind(this);
    var popUpWindow = this.popUp;
    //var painter = this.painter;
    //var seqs = this.seqs;
    //var seqStart = this.seqStart;
    //var baseStart = this.baseStart;
    $(this.canvas).mousemove(function(e) {
	    $(popUpWindow).hide();
	    //console.log(popUpWindow);
	    var rect = this.canvas.getBoundingClientRect();
		//console.log(rect.left, rect.top, rect.right, rect.bottom );
		var currentPoint = getRelCursorPosition(e, this.canvas);
		//console.log("WindowX:" + (currentPoint[0]));
		//console.log("WindowY:" + (currentPoint[1]));
		//console.log("AdjustX:" + (currentPoint[0] - rect.left));
		//console.log("AdjustY:" + (currentPoint[1] - rect.top));
    	$(popUpWindow).css('top', currentPoint[1] + moveDown).css('left', currentPoint[0] + moveLeft);
    	
      	var currentBaseHover = Math.ceil(currentPoint[0]/this.painter.cw) - this.painter.nameOffSet/this.painter.cw + this.baseStart -1;
        var currentSeqHover = Math.ceil(currentPoint[1]/this.painter.ch) + this.seqStart - 1;
        //console.log(currentBaseHover);
        //console.log(currentSeqHover);
        if(currentPoint[0] > this.painter.nameOffSet){
        	//console.log($("#info", popUpWindow)[0]);
        	$("#info", popUpWindow)[0].innerHTML = "name: " + 
        	this.seqs[currentSeqHover]["name"]
        	+ "<br>base: "  + this.seqs[currentSeqHover]["seq"][currentBaseHover] 
        	+ "<br>qual: " +  this.seqs[currentSeqHover]["qual"][currentBaseHover]
        	+ "<br>pos: " + currentBaseHover;
        }else{
        	$("#info", popUpWindow)[0].innerHTML = "name: " + 
        	this.seqs[currentSeqHover]["name"];
        }

		if(currentPoint[1] < (this.painter.nSeqs * this.painter.ch) & 
			currentPoint[0] < (this.painter.nBases * this.painter.cw)){
			$(popUpWindow).fadeIn(500);
		}else{
			$(popUpWindow).hide();
		}
    }.bind(this)).bind(this);
   };
	var currentSeqPos = 0;
	var canvas = document.getElementById('canvas');
	var canvasDiv = document.getElementById('canvasDiv1');
	var canvasDiv2 = document.getElementById('canvasDiv2');
    var context = canvas.getContext('2d');
    var popUp = document.getElementById('pop-up');
    var mainSeqData, mainData;
    var needToPaint = true;
    var baseColors = {};
    baseColors['A'] = "#ff8787";
    baseColors['G'] = "#ffffaf";
    baseColors['C'] = "#afffaf";
    baseColors['T'] = "#87afff";
    var cellWidth = 20;
    var cellHeight = 30;
    var numOfBases = Math.floor((canvas.width - cellWidth)/cellWidth);
	var numOfSeqs = Math.floor((canvas.height - cellHeight)/cellHeight);
	var baseStart = 0;
	var seqStart = 0;
	var nameOffSet = 10 * cellWidth;
	function setUpCanvas(){
		var canvas = document.getElementById('canvas');
		$(canvasDiv).width((window.innerWidth - 10) * 0.98);
		$(canvasDiv).height((window.innerHeight - 60) * 0.98);
		canvas.width = $(canvasDiv).width() * 0.98;
		canvas.height = $(canvasDiv).height() * 0.95;
		$(canvasDiv2).width((window.innerWidth - 10) * 0.98);
		$(canvasDiv2).height((window.innerHeight - 60) * 0.98);
		numOfBases = Math.floor((canvas.width - cellWidth - nameOffSet)/cellWidth);
	 	numOfSeqs = Math.floor((canvas.height - cellHeight)/cellHeight);
	};
	function updateCanvas(){
		var canvas = document.getElementById('canvas');
		var canvasDiv = document.getElementById('canvasDiv');
		var canvasDiv2 = document.getElementById('canvasDiv2');
		var changingHeight = (window.innerHeight - 60) * 0.98;
		var changingWidth = (window.innerWidth - 10) * 0.98;
		$(canvasDiv).width((window.innerWidth - 10) * 0.98);
		$(canvasDiv).height((window.innerHeight - 60) * 0.98);
		$(canvasDiv2).width((window.innerWidth - 10) * 0.98);
		$(canvasDiv2).height((window.innerHeight - 60) * 0.98);
		//$(canvasDiv2).top($(canvasDiv).height() + 10);
		canvas.width = $(canvasDiv).width() * 0.98;
		canvas.height = $(canvasDiv).height() * 0.95;
		if(changingHeight > canvas.height){
			needToPaint = true;
		}
		if(changingWidth > canvas.width){
			needToPaint = true;
		}
		numOfBases = Math.floor((canvas.width - cellWidth - nameOffSet)/cellWidth);
	 	numOfSeqs = Math.floor((canvas.height - cellHeight)/cellHeight);
	};
	function drawCircle(x, y, radius, color, borderColor){
        context.beginPath();
        context.arc(x, y, radius, 0, 2 * Math.PI, false);
        context.fillStyle = color;
        context.fill();
        context.lineWidth = 5;
        context.strokeStyle = borderColor;
        context.stroke();
    }

    function drawLine(sx, sy, ex, ey, width){
        context.beginPath();
        context.moveTo(sx, sy);
        context.lineTo(ex, ey, width);
        context.stroke();
    }

    function ajax(url, func){
        $.ajax({ url: url, dataType: 'json', async: false,
            success: function(ct){ func(ct); } });
    }

    function ajaxAsync(url, func){
        $.ajax({ url: url, dataType: 'json', async: true,
                 success: function(ct){ func(ct); } });
    }


    
    function drawSeq(index, seq, start){
    	context.textAlign = "center";
    	context.textBaseline = "middle";
    	context.font = "bold 15px 'Helvetica Neue',Helvetica, Arial, sans-serif";
    	for(var wi = start; wi - start < numOfBases; wi++){
            if(wi < seq.length){
            	context.fillStyle = baseColors[seq[wi]];
            	context.fillRect((wi - start) * cellWidth + nameOffSet, index*cellHeight, cellWidth, cellHeight);
            	context.fillStyle = "#000000";
	        	context.fillText(seq[wi], (wi -start)*cellWidth + cellWidth/2.0 + nameOffSet, index*cellHeight + cellHeight/2.0);
            }else{
            	context.strokeStyle = "#000000";
    			context.lineWidth   = 1;
            	context.fillStyle = "#EEEEEE";
            	context.fillRect((wi -start) * cellWidth + nameOffSet, index*cellHeight, cellWidth, cellHeight);
            	context.strokeRect((wi -start) * cellWidth + nameOffSet, index*cellHeight, cellWidth, cellHeight);
            }
        }
    };
    //
    function paint(seqData){
    	if(needToPaint){
    		//console.log("painting");
    		//box around seqs
    		context.strokeStyle = "#000000";
			context.lineWidth   = 1;
	    	context.strokeRect(nameOffSet, 0, cellWidth * numOfBases, cellHeight * numOfSeqs);
	    	//write names
	    	context.textAlign = "left";
	    	context.textBaseline = "middle";
	    	context.font = "bold 15px 'Helvetica Neue',Helvetica, Arial, sans-serif";
	        context.strokeStyle = "#000000";
			context.lineWidth   = 1;
	    	
	    	for(var hi = seqStart; hi -seqStart < numOfSeqs ; hi++){
	    		context.fillStyle = "#EEEEEE";
		    	context.fillRect(0, (hi - seqStart)*cellHeight, nameOffSet, cellHeight);
		    	context.strokeRect(0, (hi - seqStart)*cellHeight, nameOffSet, cellHeight);
		    	context.fillStyle = "#000000";
				context.fillText(seqData[hi]["name"], 2, (hi - seqStart)*cellHeight + cellHeight/2.0);
	    	}	    	

	    	for(var hi = seqStart; hi -seqStart < numOfSeqs ; hi++){
	    		drawSeq(hi - seqStart,
	    		 seqData[hi]["seq"], baseStart);
	    	}
	    	needToPaint = false;
    	}
    }

    function mouseWheelUp(steps){
    	
        if(seqStart > 0){
        	--seqStart;
        	$("#rightSlider").slider('value', mainData["numReads"] - seqStart - numOfSeqs);
        	needToPaint = true;
        	paint(mainSeqData["seqs"]);
        }
    }

    function mouseWheelDown(steps){
        if(seqStart < Math.max(mainData["numReads"]- numOfSeqs, 0)){
        	++seqStart;
        	$("#rightSlider").slider('value', mainData["numReads"] - seqStart - numOfSeqs);
        	needToPaint = true;
        	paint(mainSeqData["seqs"]);
        }
    }
    
    function placeBasePos(){
    	context.textAlign = "center";
    	context.textBaseline = "middle";
    	context.fillStyle = "#EEEEEE";
	    context.fillRect(nameOffSet - cellWidth, (numOfSeqs)*cellHeight + 2 , cellWidth * 2, cellHeight);
	    context.fillRect(nameOffSet - cellWidth + numOfBases * cellWidth -cellWidth, (numOfSeqs)*cellHeight + 2 , cellWidth * 2, cellHeight);
	    context.fillStyle = "#000000";
    	context.font = "bold 15px 'Helvetica Neue',Helvetica, Arial, sans-serif";
      	context.fillText(baseStart, nameOffSet, (numOfSeqs)*cellHeight +2 + cellHeight/2.0);
      	context.fillText(baseStart + numOfBases -1, nameOffSet + numOfBases * cellWidth -cellWidth, (numOfSeqs)*cellHeight +2 + cellHeight/2.0);
    }
    function addHoverBoxListener(object, fadeInBox) {
	    var moveLeft = 20;
	    var moveDown = 10;
	    //object.hover(function(e) {
	    $(object).hover(function(e) {
	      //fadeInBox.fadeIn(500);
	      $(fadeInBox).fadeIn(500);
	      //.css('top', e.pageY + moveDown)
	      //.css('left', e.pageX + moveLeft)
	      //.appendTo('body');
	    }, function() {
	      //fadeInBox.hide();
	      $(fadeInBox).hide();
	    });
	    
	    $(object).mousemove(function(e) {
		    $(fadeInBox).hide();
			var currentPoint = [e.pageX - $(object).offset().left, e.pageY - $(object).offset().top];
	    	$(fadeInBox).css('top', e.pageY + moveDown).css('left', e.pageX + moveLeft);
	    	
	      	var currentBaseHover = Math.ceil(currentPoint[0]/cellWidth) - nameOffSet/cellWidth + baseStart -1;
	        var currentSeqHover = Math.ceil(currentPoint[1]/cellHeight) + seqStart - 1;
	        if(currentPoint[0] > nameOffSet){
	        	document.getElementById("info").innerHTML = "name: " + 
	        	mainSeqData["seqs"][currentSeqHover]["name"]
	        	+ "<br>base: "  + mainSeqData["seqs"][currentSeqHover]["seq"][currentBaseHover] 
	        	+ "<br>qual:" +  mainSeqData["seqs"][currentSeqHover]["qual"][currentBaseHover]
	        	+ "<br>pos: " + currentBaseHover;
	        }else{
	        	document.getElementById("info").innerHTML = document.getElementById("info").innerHTML = "name: " + 
	        	mainSeqData["seqs"][currentSeqHover]["name"];
	        }

			if(currentPoint[1] < (numOfSeqs * cellHeight) & 
				currentPoint[0] < (numOfBases * cellWidth)){
				$(fadeInBox).fadeIn(500);
			}else{
				$(fadeInBox).hide();
			}
	    });
	  };
	//var painter = new SeqPainter(cellWidth, cellHeight, numOfSeqs, numOfBases, baseColors);
    function setUpSliders(maxSeqs, maxBases){
    	$( "#bottomSlider", canvasDiv ).css("left", nameOffSet);
    	$( "#bottomSlider", canvasDiv ).css("width", numOfBases * cellWidth);
    	$( "#rightSlider" ).css("height", numOfSeqs * cellHeight);
	    $( "#bottomSlider" ).slider({
	      range: "min",
	      min: 0,
	      max: Math.max(maxBases- numOfBases, 0),
	      value: 0,
	      slide :function(event, ui){
	      	baseStart = ui.value;
	      	needToPaint = true;
	      	paint(mainSeqData["seqs"]);
			placeBasePos();
	      }
	      });
	    $( "#rightSlider" ).slider({
	      range: "max",
	      min: 0,
	      max: Math.max(maxSeqs- numOfSeqs, 0),
	      value: maxSeqs,
	      orientation: "vertical", slide :function(event, ui){
	      	needToPaint = true;
	      	seqStart = maxSeqs - numOfSeqs - ui.value;
	      	paint(mainSeqData["seqs"]);
	      }
	    });
    }
    var mainSeqData2, mainData2;
    
    ajax('/ssv/mainSeqData', function(msd){ mainSeqData = msd; });
	ajax('/ssv/mainData', function(md){ mainData = md; });
	
	var SeqViewer = new SeqView("canvasDiv1", mainSeqData["seqs"], mainData, cellWidth, cellHeight, baseColors);
	var SeqViewer2 = new SeqView("canvasDiv2", mainSeqData["seqs"], mainData, cellWidth, cellHeight, baseColors);
	
	function init(){
		$(window).bind("resize", function(){
			SeqViewer.updateCanvas();
			SeqViewer.setUpSliders();
			SeqViewer.paint();
			SeqViewer.painter.placeBasePos(SeqViewer.context, SeqViewer.baseStart);
			
			SeqViewer2.updateCanvas();
			SeqViewer2.setUpSliders();
			SeqViewer2.paint();
			SeqViewer2.painter.placeBasePos(SeqViewer.context, SeqViewer.baseStart);
			//updateCanvas();
			//setUpSliders(mainData["numReads"], mainData["maxLen"] );
			//painter.nSeqs = numOfSeqs;
			//painter.nBases = numOfBases;
			//SeqViewer.painter.paintSeqs(context, mainSeqData["seqs"], seqStart, baseStart);
			//paint(mainSeqData["seqs"]);
			//placeBasePos();
		});
		SeqViewer.setUp();
		SeqViewer.paint();
		SeqViewer2.setUp();
		SeqViewer2.paint();
		//setUpCanvas();
		
		//setUpSliders(mainData["numReads"],mainData["maxLen"] );
		//painter.nSeqs = numOfSeqs;
		//painter.nBases = numOfBases;
		//painter.paintSeqs(context, mainSeqData["seqs"], seqStart, baseStart);
		//paint(mainSeqData["seqs"]);
		console.log(numOfBases);
		console.log(mainData["maxLen"]);
		console.log(mainData["maxLen"] - numOfBases);
		console.log(numOfSeqs);
		console.log(mainData["numReads"]);
		console.log(mainData["numReads"] - numOfSeqs);
		//addMouseScrollListener(canvas, mouseWheelUp,mouseWheelDown);
		//addClickListener(canvas);
		//placeBasePos();
		//addHoverBoxListener(canvas, popUp);
		
	}
	init();


    function addClickListener(canvas){
        var clicked = function(e){
            var pt = getCursorPosition(e);
            currentBase = Math.ceil(pt[0]/cellWidth) - nameOffSet/cellWidth + baseStart -1;
            currentSeq = Math.ceil(pt[1]/cellHeight) + seqStart - 1;
            console.log(pt);
            console.log(currentSeq);
            console.log(currentBase);
            console.log(mainSeqData["seqs"][currentSeq]["name"]);
            console.log(mainSeqData["seqs"][currentSeq]["seq"][currentBase]);
            console.log(mainSeqData["seqs"][currentSeq]["qual"][currentBase]);
        };
        canvas.addEventListener("mousedown", clicked, false);
    }

});
