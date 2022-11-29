(function () {
  Drupal.behaviors.ereolenClearFields = {
    attach: function (context, settings) {
      'use strict';

      var checkbox = document.getElementsByClassName('js-adgangsplatformen-checkbox');
      if (checkbox.length > 0) {
        // Ajax load content, so it may not be there on first check.
        checkbox[0].addEventListener('click', function(event) {
          if (event.target.checked) {
            setCookie('ssopupop', 'disabled', 30);
          }
          else {
            eraseCookie('ssopupop');
          }
        });
      }
    }
  };

  // See https://stackoverflow.com/questions/14573223/set-cookie-and-get-cookie-with-javascript
  // about these cookie helper functions.
  function setCookie(name,value,days) {
    var expires = "";
    if (days) {
      var date = new Date();
      date.setTime(date.getTime() + (days*24*60*60*1000));
      expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "")  + expires + "; path=/";
  }

  function eraseCookie(name) {
    document.cookie = name +'=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;';
  }

}());
