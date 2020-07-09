/**
 * @file Detect device information.
 *   https://medium.com/creative-technology-concepts-code/detect-device-browser-and-version-using-javascript-8b511906745
 */
(function () {
  Drupal.behaviors.ereolenSupport = {
    attach: function (context, settings) {
      'use strict';

      let module = {
        options: [],
        header: [navigator.platform, navigator.userAgent, navigator.appVersion, navigator.vendor, window.opera],
        dataos: [
          { name: 'Windows Phone', value: 'Windows Phone', version: 'OS' },
          { name: 'Windows', value: 'Win', version: 'NT' },
          { name: 'iPhone', value: 'iPhone', version: 'OS' },
          { name: 'iPad', value: 'iPad', version: 'OS' },
          { name: 'Kindle', value: 'Silk', version: 'Silk' },
          { name: 'Android', value: 'Android', version: 'Android' },
          { name: 'PlayBook', value: 'PlayBook', version: 'OS' },
          { name: 'BlackBerry', value: 'BlackBerry', version: '/' },
          { name: 'Macintosh', value: 'Mac', version: 'OS X' },
          { name: 'Linux', value: 'Linux', version: 'rv' },
          { name: 'Palm', value: 'Palm', version: 'PalmOS' }
        ],
        databrowser: [
          { name: 'Chrome', value: 'Chrome', version: 'Chrome' },
          { name: 'Firefox', value: 'Firefox', version: 'Firefox' },
          { name: 'Safari', value: 'Safari', version: 'Version' },
          { name: 'Internet Explorer', value: 'MSIE', version: 'MSIE' },
          { name: 'Opera', value: 'Opera', version: 'Opera' },
          { name: 'BlackBerry', value: 'CLDC', version: 'CLDC' },
          { name: 'Mozilla', value: 'Mozilla', version: 'Mozilla' }
        ],
        init: function () {
          let agent = this.header.join(' '),
            os = this.matchItem(agent, this.dataos),
            browser = this.matchItem(agent, this.databrowser);

          return { os: os, browser: browser };
        },
        matchItem: function (string, data) {
          let i = 0,
            j = 0,
            html = '',
            regex,
            regexv,
            match,
            matches,
            version;

          for (i = 0; i < data.length; i += 1) {
            regex = new RegExp(data[i].value, 'i');
            match = regex.test(string);
            if (match) {
              regexv = new RegExp(data[i].version + '[- /:;]([\\d._]+)', 'i');
              matches = string.match(regexv);
              version = '';
              if (matches) { if (matches[1]) { matches = matches[1]; } }
              if (matches) {
                matches = matches.split(/[._]+/);
                for (j = 0; j < matches.length; j += 1) {
                  if (j === 0) {
                    version += matches[j] + '.';
                  } else {
                    version += matches[j];
                  }
                }
              } else {
                version = '0';
              }
              return {
                name: data[i].name,
                version: parseFloat(version)
              };
            }
          }
          return { name: 'unknown', version: 0 };
        }
      };

      let e = module.init();
      let deviceSelect = document.getElementById('edit-ereolen-support-device');
      let productSelect = document.getElementById('edit-ereolen-support-product');

      deviceSelect.addEventListener('change', function(){
        // @todo Change comparison value when dropdown is defined.
        if (productSelect.text == 'a') {
          let deviceInput = document.getElementById('edit-ereolen-support-model');
          let operatingSystemInput = document.getElementById('edit-ereolen-support-operating-system');
          if(document.forms['ereolen_support_form']['ereolen_support_model'].value.length === 0) {
            deviceInput.value = navigator.appVersion;
          }
          if(document.forms['ereolen_support_form']['ereolen_support_operating_system'].value.length === 0) {
            operatingSystemInput.value = e.os.name + ': ' + e.os.version + '(' + e.browser.name + ': ' + e.browser.version + ')';
          }
        }
      });
    }
  }
}());
