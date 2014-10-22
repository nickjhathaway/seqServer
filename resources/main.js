// based on example from http://thecodeplayer.com/walkthrough/html5-game-tutorial-make-a-snake-game-using-html5-canvas-jquery
$(document).ready(function(){
    var canvas = document.getElementById('myCanvas');
    var context = canvas.getContext('2d');

    canvas.width = window.innerWidth * 0.98;
    canvas.height = window.innerHeight * 0.98; // accomodate titlebar

    var centerX = canvas.width / 2;
    var centerY = canvas.height / 2;
    var radius = 10;

    function drawCircle(x, y, radius, color, borderColor){
        context.beginPath();
        context.arc(x, y, radius, 0, 2 * Math.PI, false);
        context.fillStyle = color;
        context.fill();
        context.lineWidth = 5;
        context.strokeStyle = borderColor;
        context.stroke();
    }

    drawCircle(canvas.width /2, canvas.height /2, 50, "red", "green");
    drawCircle(100, 200, 50, "blue", "red");
    drawCircle(800, 400, 10, "yellow", "purple");

    function drawLine(sx, sy, ex, ey, width){
        context.beginPath();
        context.moveTo(sx, sy);
        context.lineTo(ex, ey, width);
        context.stroke();
    }

    drawLine(100, 200, 800, 400, 50);

    function ajax(url, func){
        $.ajax({ url: url, dataType: 'json', async: false,
            success: function(ct){ func(ct); } });
    }

    function ajaxAsync(url, func){
        $.ajax({ url: url, dataType: 'json', async: true,
                 success: function(ct){ func(ct); } });
    }

    ajax('/evt/mainData', function(md){
        $.each(md["circles"], function(i, c) {
            console.log(c);
            drawCircle(c["x"], c["y"], c["radius"], c["color"],
                       c["borderColor"]);
        });
    });

    function addClickListener(canvas, regions){
        this.getCursorPosition = function(event) {
            // from http://stackoverflow.com/a/5417934
            var canoffset = $(canvas).offset();
            var x = event.clientX + document.body.scrollLeft +
                document.documentElement.scrollLeft - Math.floor(canoffset.left);
            var y = event.clientY + document.body.scrollTop +
                document.documentElement.scrollTop - Math.floor(canoffset.top) + 1;
            return [x,y];
        }
        this.clickInBBox = function(x, y, bbox){
            var b = bbox;
            // pad
            b[0] -= 5; b[1] -= 5;
            b[2] += 5; b[3] += 5;
            if(x > b[0] && x < b[2] && y > b[1] && y < b[3]){
                return true;
            }
            return false;
        }
        this.clicked = function(e){
            var pt = getCursorPosition(e);
            for(var i = 0; i < regions.length; ++i){
                var name = regions[i][0];
                var bbox = screenBoxes[name];
                if(clickInBBox(pt[0], pt[1], bbox)){
                    regions[i][1]();
                    return;
                }
            }
        }
        canvas.addEventListener("mousedown", clicked, false);
    }

})
