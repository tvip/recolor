function recolor() {
  console.log('welcome to Recolor')

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

      redirect_stream('console.log', '/recolor/stdout/clock.png')
      redirect_stream('console.error', '/recolor/stderr/clock.png')
    })
  })
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
