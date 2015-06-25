function dragndrop() {

  console.log('session: ' + $.cookie('session_id'))

  console.log('drag n drop is ready')
  var dropZone = $('#dropZone'),
      maxFileSize = 10000000  // максимальный размер фалйа - 10 мб.

  // Проверка поддержки браузером
  if (typeof(window.FileReader) == 'undefined') {
    dropZone.text('Не поддерживается браузером!')
    dropZone.addClass('error')
  }

  // Добавляем класс hover при наведении
  dropZone[0].ondragover = function() {
    console.log('ondragover')
    dropZone.text('')
    dropZone.removeClass('error')
    dropZone.removeClass('drop')
    dropZone.addClass('hover')
    return false
  }

  // Убираем класс hover
  dropZone[0].ondragleave = function() {
    console.log('ondragleave')
    dropZone.removeClass('hover')
    return false
  }

  // Обрабатываем событие Drop
  dropZone[0].ondrop = function(event) {
    console.log('ondrop')
    event.preventDefault()
    dropZone.removeClass('hover')
    dropZone.addClass('drop')

    var file = event.dataTransfer.files[0]

    // Проверяем размер файла
    if (file.size > maxFileSize) {
      dropZone.text('Файл слишком большой!')
      dropZone.addClass('error')
      return false
    }

    // Создаем запрос
    var xhr = new XMLHttpRequest()
    xhr.upload.addEventListener('progress', uploadProgress, false)
    xhr.onreadystatechange = stateChange
    xhr.open('PUT', '/recolor/upload')
    xhr.setRequestHeader('X-FILE-NAME', file.name)
    xhr.setRequestHeader('Content-Type', 'image/*')
    xhr.send(file)
  }

  // Показываем процент загрузки
  function uploadProgress(event) {
    console.log('uploadProgress')
    var percent = parseInt(event.loaded / event.total * 100)
    dropZone.text('Загрузка: ' + percent + '%')
  }

  // Пост обрабочик
  function stateChange(event) {
    console.log('stateChange')
    if (event.target.readyState == 4) {
      if (event.target.status == 200) {
        dropZone.text('Загрузка успешно завершена!')
      } else {
        dropZone.text('Произошла ошибка!')
        dropZone.removeClass('drop')
        dropZone.addClass('error')
      }
    }
  }

}