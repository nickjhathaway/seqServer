


    
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
    	seqContext.font = "bold 15px Arial, sans-serif";
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
	    	seqContext.strokeRect(this.nameOffSet, 0,this.cw * this.nBases,
	    		 Math.min(this.ch * this.nSeqs,this.ch * seqData.length ));
	    	//write names
	    	seqContext.textAlign = "left";
	    	seqContext.textBaseline = "middle";
	    	seqContext.font = "bold 15px Arial, sans-serif";
	        seqContext.strokeStyle = "#000000";
			seqContext.lineWidth   = 1;
	    	for(var hi = sStart;( hi -sStart < this.nSeqs ) && (hi < seqData.length); hi++){
	    		console.log(hi);
	    		console.log(this.nSeqs);
	    		seqContext.fillStyle = "#EEEEEE";
		    	seqContext.fillRect(0, (hi - sStart)*this.ch, this.nameOffSet, this.ch);
		    	seqContext.strokeRect(0, (hi - sStart)*this.ch, this.nameOffSet, this.ch);
		    	seqContext.fillStyle = "#000000";
				seqContext.fillText(seqData[hi]["name"], 2, (hi - sStart)*this.ch + this.ch/2.0);
	    	}	    	
	    	for(var hi = sStart;( hi -sStart < this.nSeqs ) && (hi < seqData.length) ; hi++){
	    		this.paintSeq(seqContext, hi - sStart,
	    		 seqData[hi]["seq"], bStart);
	    	}
	    	this.needToPaint = false;
	    	//console.log("end-painting");
    	}
	};
	//paint seq position
	SeqPainter.prototype.placeBasePos = function(seqContext, sStart, bStart){
    	seqContext.textAlign = "center";
    	seqContext.textBaseline = "middle";
    	seqContext.fillStyle = "#EEEEEE";
	    seqContext.fillRect(this.nameOffSet - this.cw, (this.nSeqs)*this.ch + 2 , this.cw * 2, this.ch);
	    seqContext.fillRect(this.nameOffSet - this.cw + this.nBases * this.cw -this.cw, (this.nSeqs)*this.ch + 2 , this.cw * 2, this.ch);
	    seqContext.fillRect(this.nameOffSet + this.nBases * this.cw + 2, 0 , this.cw * 2, this.ch);
	    seqContext.fillRect(this.nameOffSet + this.nBases * this.cw + 2, (this.nSeqs)*this.ch - this.ch , this.cw * 2, this.ch);
	    seqContext.fillStyle = "#000000";
    	seqContext.font = "bold 15px Arial, sans-serif";
      	seqContext.fillText(bStart, this.nameOffSet, (this.nSeqs)*this.ch +2 + this.ch/2.0);
      	seqContext.fillText(bStart + this.nBases -1, this.nameOffSet + this.nBases * this.cw -this.cw, (this.nSeqs)*this.ch +2 + this.ch/2.0);
   		seqContext.fillText(sStart, this.nameOffSet + this.nBases * this.cw + this.cw + 2, this.ch/2.0  );
   		seqContext.fillText(sStart + this.nSeqs - 1, this.nameOffSet + this.nBases * this.cw + this.cw + 2, (this.nSeqs)*this.ch - this.ch+ this.ch/2.0  );
   };
   
   SeqPainter.prototype.paintSelectedSeq = function(seqContext,seq, currentBase){
   		seqContext.font = "bold 15px Arial, sans-serif";
   		seqContext.textAlign = "left";
    	seqContext.textBaseline = "middle";
   		var logInfo = "name: " + 
        	seq["name"]
        	+ " base: "  + seq["seq"][currentBase] 
        	+ " qual: " +  seq["qual"][currentBase]
        	+ " pos: " + currentBase;
        var tWidth = seqContext.measureText(logInfo).width;
        seqContext.fillStyle = "#FFFFFF";
	    seqContext.fillRect(this.nameOffSet + this.cw + 2, (this.nSeqs)*this.ch + 2 , this.nBases * this.cw, this.ch);
        seqContext.fillStyle = "#EEEEEE";
	    seqContext.fillRect(this.nameOffSet + this.cw + 2, (this.nSeqs)*this.ch + 2 , tWidth, this.ch);
	    seqContext.fillStyle = "#000000";
	    console.log(tWidth);
	    seqContext.fillText(logInfo,this.nameOffSet + this.cw + 2, (this.nSeqs)*this.ch + 2  + this.ch/2 );
	    
   };
	
	function SeqView(viewName, seqData, cellWidth, cellHeight, baseColors, qualChartName){
		//need to add style and html, currently just there
		//retrieve html elements 
		this.masterDiv = document.getElementById(viewName);
		this.canvas = $("#canvas", this.masterDiv)[0];
		this.context = this.canvas.getContext('2d');
		this.rSlider = $("#rightSlider", this.masterDiv)[0];
		this.bSlider = $("#bottomSlider", this.masterDiv)[0];
		this.popUp = $("#pop-up", this.masterDiv)[0];
		this.sel = $("#select", this.masterDiv)[0];
		this.seqData = seqData;
		this.seqStart = 0;
		this.baseStart = 0;
		this.currentSeq = 0;
		this.currentBase = 0;
		// set up of sizes
		$(this.masterDiv).width((window.innerWidth - 10) * 0.98);
		$(this.masterDiv).height((window.innerHeight - 60) * 0.98);
		this.canvas.width = $(this.masterDiv).width() * 0.98;
		this.canvas.height = $(this.masterDiv).height() * 0.95;
		var nameOffSet = 10 * cellWidth;
		var numOfBases = Math.floor((this.canvas.width - cellWidth - nameOffSet)/cellWidth);
	 	var numOfSeqs = Math.min(Math.floor((this.canvas.height - cellHeight)/cellHeight), this.seqData["seqs"].length);
	 	console.log(numOfSeqs);
	 	console.log(Math.floor((this.canvas.height - cellHeight)/cellHeight));
	 	console.log(this.seqData["seqs"].length);
		this.painter = new SeqPainter(cellWidth, cellHeight, numOfSeqs, numOfBases, nameOffSet, baseColors);
		//this.seqs = seqs;

		this.chart = c3.generate({
			bindto: qualChartName,
		    data: {
		        json: {
		            qual: this.seqData["seqs"][this.currentSeq]["qual"]
		        }
		    }, 
			grid: {
		        y: {
		            lines: [{value: 20}]
		        }
		    }
		});
		//
	};
	
	SeqView.prototype.setUp = function(){
		this.setUpCanvas();
		this.setUpSliders();
		this.setUpListeners();
		this.setSelector();
	};
	
	SeqView.prototype.setUpCanvas = function(){
		$(this.masterDiv).width((window.innerWidth - 10) * 0.98);
		var maxPossHeight = this.painter.ch * (this.seqData["seqs"].length + 4);
		$(this.masterDiv).height(Math.min((window.innerHeight - 60) * 0.80, maxPossHeight));
		this.canvas.width = $(this.masterDiv).width() * 0.96;
		this.canvas.height = $(this.masterDiv).height() * 0.95;
		this.painter.nBases = Math.floor((this.canvas.width - this.painter.cw - this.painter.nameOffSet)/this.painter.cw);
	 	this.painter.nSeqs = Math.min(Math.floor((this.canvas.height - this.painter.ch)/this.painter.ch),this.seqData["seqs"].length);
	};
	
	SeqView.prototype.updateCanvas = function(){
		var changingHeight = (window.innerHeight - 60) * 0.80;
		var changingWidth = (window.innerWidth - 10) * 0.98;
		$(this.masterDiv).width((window.innerWidth - 10) * 0.98);
		var maxPossHeight = this.painter.ch * (this.seqData["seqs"].length + 4);
		$(this.masterDiv).height(Math.min((window.innerHeight - 60) * 0.80, maxPossHeight));
		this.canvas.width = $(this.masterDiv).width() * 0.96;
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
		this.painter.paintSeqs(this.context, this.seqData["seqs"], this.seqStart, this.baseStart);
		this.painter.placeBasePos(this.context, this.seqStart, this.baseStart);
		this.paintSelectedSeq();
		this.setSelector();
	};
	
	SeqView.prototype.paintSelectedSeq = function(){
		this.painter.paintSelectedSeq(this.context, this.seqData["seqs"][this.currentSeq], this.currentBase );
	};
	
	SeqView.prototype.setSelector = function(){
		if(this.currentBase >= this.baseStart && this.currentSeq >=this.seqStart){
			$(this.sel).css('top', (this.currentSeq -this.seqStart) *this.painter.ch -1);
			$(this.sel).css('left', (this.currentBase - this.baseStart) *this.painter.cw + this.painter.nameOffSet -1);
			$(this.sel).show();
		}else{
			$(this.sel).hide();
		}
		/*console.log("setSelector");
		console.log(this.sel);
		console.log(this.currentBase);
		console.log(this.currentSeq );
		console.log(this.currentSeq *this.painter.ch);
		console.log(this.currentBase *this.painter.cw + this.painter.nameOffSet );*/
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
        this.paintSelectedSeq();
        console.log(pt);
        console.log(this.currentSeq);
        console.log(this.currentBase);
        console.log(this.seqData["seqs"][this.currentSeq]["name"]);
        console.log(this.seqData["seqs"][this.currentSeq]["seq"][this.currentBase]);
        console.log(this.seqData["seqs"][this.currentSeq]["qual"][this.currentBase]);
        this.setSelector();
        var currentQual = this.seqData["seqs"][this.currentSeq]["qual"];
		this.chart.load({
	        json: {
	            qual: this.seqData["seqs"][this.currentSeq]["qual"]
	        }
	    });

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
    //var seqs = this.seqData["seqs"];
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
        	this.seqData["seqs"][currentSeqHover]["name"]
        	+ "<br>base: "  + this.seqData["seqs"][currentSeqHover]["seq"][currentBaseHover] 
        	+ "<br>qual: " +  this.seqData["seqs"][currentSeqHover]["qual"][currentBaseHover]
        	+ "<br>pos: " + currentBaseHover;
        }else{
        	$("#info", popUpWindow)[0].innerHTML = "name: " + 
        	this.seqData["seqs"][currentSeqHover]["name"];
        }

		if(currentPoint[1] < (this.painter.nSeqs * this.painter.ch) && 
			currentPoint[0] < ((this.painter.nBases * this.painter.cw) + this.painter.nameOffSet)){
			$(popUpWindow).fadeIn(500);
		}else{
			$(popUpWindow).hide();
		}
    }.bind(this)).bind(this);
   };

