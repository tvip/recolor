var images = new Array()
function Image(file) {
  var URL = window.URL || window.webkitURL
  var name = file.name
  this.name = name

  if (!images[name]) {
    var table = document.getElementById("images_table")
    var row = table.insertRow(0)
    var cell1 = row.insertCell(0)
    var cell2 = row.insertCell(1)
    var img1 = document.createElement("img")
    img1.id = 'orig_' + name
    var img2 = document.createElement("img")
    img2.id = 'res_' + name
    img2.src = ''
    cell1.appendChild(img1)
    cell2.appendChild(img2)
  }

  var img1 = document.getElementById('orig_' + name)
  var img1_url = URL.createObjectURL(file)
  console.log('img1_url ' + img1 + ' ' + img1_url)
  img1.onload = function() {
    URL.revokeObjectURL(img1_url);
  }

  img1.src = img1_url

  images[name] = this
}

function rgbToHex (r, g, b)
{  
    r = Math.round(255*r).toString(16);
    g = Math.round(255*g).toString(16);
    b = Math.round(255*b).toString(16);
    
    if (r.length == 1) r = '0' + r;
    if (g.length == 1) g = '0' + g;
    if (b.length == 1) b = '0' + b;
    
    return (r + g + b).toUpperCase();
}

function matrix_changed() {
  console.log('matrix changed')
  
  var colors_table = document.getElementById('colors_table')
  while (colors_table.rows.length) {
    colors_table.deleteRow(0)
  }
  
  var splitted = $('#matrix').val().split(/[\s]+/)
  var color = new Array()
  var row
  
  for ( var i in splitted ) {

    if (splitted[i].length) {
      color.push( Number(splitted[i]) )
    }
    if (color.length == 3) {
      if ((row?row.cells.length:0) % 2 == 0) {
        row = colors_table.insertRow(colors_table.rows.length)
      }
      
      console.log('rows: ' + colors_table.rows.length + '  cells: ' + row.cells.length)
      var cell = row.insertCell(row.cells.length)
      var div = document.createElement('div')
      div.className = "color-box"
      cell.appendChild(div)
      
      var hex = rgbToHex(color[0], color[1], color[2])
      $(div).css('background-color', '#'+hex)
      
      $(div).colpick({
        color: hex,
        onChange:function(hsb,hex,rgb,el,bySetColor) {
          $(el).css('background-color', '#'+hex)
        },
        onSubmit:function(hsb,hex,rgb,el) {
          $(el).css('background-color', '#'+hex)
          $(el).colpickHide()
        }
      })
      
      console.log('HEX: ' + rgbToHex(color[0], color[1], color[2]))
      color = new Array()
    }
  }
}

function recolor() {
  console.log('welcome to Recolor')
  
  $('.color-box').colpick({
    //color:'ff8800',
    onSubmit:function(hsb,hex,rgb,el) {
      $(el).css('background-color', '#'+hex);
      $(el).colpickHide();
    }
  })
  .css('background-color', '#ff8800');
  
  // TODO: подгружать перекрашенные картинки без нажатия на кнопку 
  $("#matrix_form").submit(function(event) {

    /* stop form from submitting normally */
    event.preventDefault()

    /* get some values from elements on the page: */
    var $form = $( this ),
          url = $form.attr( 'action' )

    /* Send the data using post */
    var posting = $.post( url, { matrix: $('#matrix').val() } )

    /* Alerts the results */
    posting.done(function( data ) {
      //$('#img').attr('src', 'data:image/png;base64,' + data)
      console.log('success')
      console.log(data)

      for (var fname in images) {
        console.log('KEY ' + fname)

        //redirect_stream('console.log', '/recolor/stdout/' + fname)
        //redirect_stream('console.error', '/recolor/stderr/' + fname)

        var res_img = document.getElementById('res_' + fname)
        res_img.src = '/recolor/image/' + fname + '?timestamp=' + new Date().getTime()
        
        /*var xhr = new XMLHttpRequest()
        xhr.onreadystatechange = function(data) {
          //console.log('Recolor onreadystatechange' + xhr.responseText)
          console.log('res_img ' + fname + ' ' + res_img )
          res_img.src = 'data:image/png;base64,' + xhr.responseText
          if (event.target.readyState == 4) {
            if (event.target.status == 200) {
              console.log('RECOLORED')
              console.log('responseText' + xhr.responseText)
            }
            else {
              console.error('RELORED')
            }
          }
        }

        xhr.open('get', '/recolor/base64/' + fname)
        xhr.send(null)*/
      }

    })
  })
}

function dummy_stream() {
  if (!!window.EventSource) {
    eval( printAllKeyValues( 'EventSource' ) )

    var source = new EventSource('/recolor/thing')
    eval( printAllKeyValues( 'source' ) )

    source.addEventListener('message', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' MESSAGE:' + e.data)
    }, false)

    source.addEventListener('open', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' OPEN: ' + e.data)
    }, false)

    source.addEventListener('error', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' ERROR: ' + e.data)
      source.close()
    }, false)

  } else {
    console.log('Error: can\'t stream recolor-tool log')
  }
}

function redirect_stream(callback, url) {

  if (!!window.EventSource) {

    var source = new EventSource(url)

    source.addEventListener('message', function(e) {
      eval(callback + '(window.atob(e.data))')
    	//console.error(e.data)
    }, false)

    source.addEventListener('open', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' OPEN: ' + e.data)
    }, false)

    source.addEventListener('error', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' ERROR: ' + e.data)
      source.close()
    }, false)

  } else {
    console.error('Your browser don\'t support text streaming')
  }
}

function redirect_stdout() {
  redirect_stream('console.log', '/recolor/stdout')
}

function redirect_stderr() {
  redirect_stream('console.error', '/recolor/stderr')
}

function stream_stdout() {
  redirect_stream('console.log', '/streaming/stdout')
}

function stream_stderr() {
  redirect_stream('console.error', '/streaming/stderr')
}
