function recolor() {
  console.log('welcome to Recolor')

  if (!!window.EventSource) {
    var source = new EventSource('..')

    source.addEventListener('message', function(e) {
      console.log('RECOLOR_TOOL: LOG:' + e.data);
    }, false)

    source.addEventListener('open', function(e) {
      console.log('RECOLOR_TOOL: connection established')
    }, false)

    source.addEventListener('error', function(e) {
      if (e.readyState == EventSource.CLOSED) {
        console.log('RECOLOR_TOOL: connection closed')
      }
      else {
        console.log('RECOLOR_TOOL: connection error')
      }
    }, false)

  } else {
    console.log('Error: can\'t stream recolor-tool log')
  }
}