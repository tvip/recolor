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
      color.push( Math.round(255 * Number(splitted[i]) ) )
    }
    if (color.length == 3) {
      if ((row?row.cells.length:0) % 2 == 0) {
        row = colors_table.insertRow(colors_table.rows.length)
      }
      
      //console.log('rows: ' + colors_table.rows.length + '  cells: ' + row.cells.length)
      var cell = row.insertCell(row.cells.length)
      cell.className = 'color-box'
      
      var hex = RGBToHex(color[0], color[1], color[2])
      $(cell).css('background-color', '#'+hex)
      
      $(cell).colpick({
        color: hex,
        onChange:function(hsb,hex,rgb,el,bySetColor) {
          $(el).css('background-color', '#'+hex)
          setNeedsPreview()
        },
        submit:0,
        onSubmit:function(hsb,hex,rgb,el) {
          $(el).css('background-color', '#'+hex)
          $(el).colpickHide()
          $('#matrix').val(matrix_from_colors())
        }
      })
      
      //console.log('HEX: ' + RGBToHex(color[0], color[1], color[2]))
      color = new Array()
    }
  }
}

var needs_preview = false
var lock_preview = false
function preview() {
  var matrix = matrix_from_colors()
  console.log( matrix )
  recolor( matrix )
  $('#matrix').val(matrix_from_colors())
  
  needs_preview = false
  lock_preview = true
  
  setTimeout(function() {
    lock_preview = false
    if (needs_preview) {
      preview()
    }
  }, 500)
}

function setNeedsPreview() {
  if (lock_preview) {
    needs_preview = true
  }
  else {
    preview()
  }
}

function matrix_from_colors() {
  var matrix = ''
  var colors_table = document.getElementById('colors_table')
  
  var row_separator = ''
  for (var i = 0, row; row = colors_table.rows[i]; i++) {
    var col_separator = ''
    matrix += row_separator
    for (var j = 0, cell; cell = row.cells[j]; j++) {
      var color = parseCSSColor( $(cell).css('background-color') )
      matrix += col_separator + sprintf( '%5.3f %5.3f %5.3f', color[0]/255, color[1]/255, color[2]/255 )
      col_separator = ' '
    }
    row_separator = '\n'
  }
  
  return matrix
}

function recolor(matrix) {
  var url = '/recolor/matrix'
  
  /* Send the data using post */
  var posting = $.post( url, { matrix: matrix } )

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
    }

  })
}

var w_r = 0.299
var w_b = 0.114
var w_g = 1 - w_r - w_b
var u_max = 0.436
var v_max = 0.615

function yuv2rgb(yuv) {
  var y = yuv[0], u = yuv[1], v = yuv[2]
  return [
    y + v * (1 - w_r) / v_max,
    y - u * w_b * (1 - w_b) / (u_max * w_g) - v * w_r * (1 - w_r) / (v_max * w_g),
    y + u * (1 - w_b) / u_max
  ]
}

function rgb2yuv(rgb) {
  var r = rgb[0], g = rgb[1], b = rgb[2]
  var y = w_r * r + w_g * g + w_b * b
  return [
    y,
    u_max * ((b - y) / (1 - w_b)),
    v_max * ((r - y) / (1 - w_r))
  ]
}

function draw_yuv(y) {
  var canvas = document.getElementById('yuv-1')
  var ctx = canvas.getContext('2d')
  var id = ctx.createImageData(canvas.width, canvas.height)
  var d = id.data
  for (var j = 0; j < canvas.height; ++j) {
    var v = v_max * (2 * j/canvas.height - 1)
    for (var i = 0; i < canvas.width; ++i) {
      var offset = 4*(j*canvas.width + i)
      var u = u_max * (2 * i/canvas.width - 1)
      var rgb = yuv2rgb([y, u, v])
      d[offset + 0] = 255 * rgb[0]
      d[offset + 1] = 255 * rgb[1]
      d[offset + 2] = 255 * rgb[2]
      d[offset + 3] = 255
    }
  }
  ctx.putImageData(id, 0, 0);
  //alert(yuv2rgb(rgb2yuv([0.5,0.6,0.8])))
}

function recolor_init() {
  console.log('welcome to Recolor')

  var slider = document.getElementById('brightness-value')
  var label = document.getElementById('brightness-label')
  slider.addEventListener('change', function(e) {
    label.innerHTML = slider.value
    draw_yuv(parseFloat(slider.value))
  })

  slider.value = 0.5
  var event = new CustomEvent('change', { "detail": "Example of an event" })
  slider.dispatchEvent(event)

  // TODO: подгружать перекрашенные картинки без нажатия на кнопку 
  $("#matrix_form").submit(function(event) {

    /* stop form from submitting normally */
    event.preventDefault()

    /* get some values from elements on the page: */
    var $form = $( this )
    
    recolor( $('#matrix').val() )
  })
  
  matrix_changed()
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
