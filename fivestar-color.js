// $Id$

if (Drupal.jsEnabled) {
  $(document).ready(function () {
    var form = $('#fivestar_color_scheme_form .color-form');
    var inputs = [];
    var focused = null;
    var radios = document.forms['fivestar-settings']['fivestar_widget'];
    var colorRadios = $('.fivestar-color-widgets input');
    var typeSelect = document.getElementById('edit-fivestar-color-type');

    // Add Farbtastic
    $(form).prepend('<div id="placeholder"></div>');
    var farb = $.farbtastic('#placeholder');

    // Decode reference colors to HSL
    var reference = Drupal.settings.fivestar.reference;
    for (i in reference) {
      if (reference[i]) {
        reference[i] = farb.RGBToHSL(farb.unpack(reference[i]));
      }
    }

    // Set up colorscheme selector
    $('#edit-scheme', form).change(function () {
      var colors = this.options[this.selectedIndex].value;
      if (colors != '') {
        colors = colors.split(',');
        for (i in colors) {
          callback(inputs[i], colors[i], false, true);
        }
        preview();
      }
    });

    // Setup radio buttons.
    $(radios).change(function() {
      $(colorRadios).each(function() {
        var widgetName = this.value.replace(/.*?\/([^\/]+)\.css/, '$1');
        var fivestarWidget = document.getElementById('fivestar-preview-' + widgetName);
        $('.star, .star a, .cancel, .cancel a', fivestarWidget).css('background-image', '');
      });
      preview();
    });

    // Setup color display select.
    $(typeSelect).change(toggleGradient);

    // Add document mouseup handlers. This prevents making many AJAX requests
    // while the color picker is being dragged around.
    $('*', $('#placeholder .farbtastic')).bind('mouseup', function() {
      preview();
    });

    /**
     * Show or hide the second set of colors.
     */
    function toggleGradient() {
      if ($(typeSelect).val() == 'solid') {
        $(inputs).filter(':odd').parent().hide()
        $('.lock', form).hide();
      }
      else {
        $(inputs).parent().show();
        $('.lock', form).show();
      }
      preview();
    }

    /**
     * Render the preview.
     */
    function preview() {
      var current = $(colorRadios).filter(':checked').val();
      if (!current) {
        return;
      }

      var widgetName = current.replace(/.*?\/([^\/]+)\.css/, '$1');
      var fivestarWidget = document.getElementById('fivestar-preview-' + widgetName);
      var inputValues = new Array();
      for (var n in inputs) {
        inputValues.push(inputs[n].value.replace(/#| /, '').replace(/^$/, 'transparent').replace(Drupal.settings.fivestar.transparent, 'transparent'));
      }

      // Star images.
      var time = new Date();
      $('.star, .star a', fivestarWidget).css('background-image', 'url(' + Drupal.settings.fivestar.colorPreview + '/' + inputValues.join('/') + '/' + widgetName + '/' + typeSelect.value + '/star.png?o=' + time.getTime() + ')');
      $('.cancel, .cancel a', fivestarWidget).css('background-image', 'url(' + Drupal.settings.fivestar.colorPreview + '/' + inputValues.join('/') + '/' + widgetName + '/' + typeSelect.value + '/cancel.png?o=' + time.getTime() + ')');
    }

    /**
     * Shift a given color, using a reference pair (ref in HSL).
     *
     * This algorithm ensures relative ordering on the saturation and luminance
     * axes is preserved, and performs a simple hue shift.
     *
     * It is also symmetrical. If: shift_color(c, a, b) == d,
     *                        then shift_color(d, b, a) == c.
     */
    function shift_color(given, ref1, ref2) {
      // Convert to HSL
      given = farb.RGBToHSL(farb.unpack(given));

      // Hue: apply delta
      given[0] += ref2[0] - ref1[0];

      // Saturation: interpolate
      if (ref1[1] == 0 || ref2[1] == 0) {
        given[1] = ref2[1];
      }
      else {
        var d = ref1[1] / ref2[1];
        if (d > 1) {
          given[1] /= d;
        }
        else {
          given[1] = 1 - (1 - given[1]) * d;
        }
      }

      // Luminance: interpolate
      if (ref1[2] == 0 || ref2[2] == 0) {
        given[2] = ref2[2];
      }
      else {
        var d = ref1[2] / ref2[2];
        if (d > 1) {
          given[2] /= d;
        }
        else {
          given[2] = 1 - (1 - given[2]) * d;
        }
      }

      return farb.pack(farb.HSLToRGB(given));
    }

    /**
     * Callback for Farbtastic when a new color is chosen.
     */
    function callback(input, color, propagate, colorscheme) {
      // Set background/foreground color
      $(input).css({
        backgroundColor: color,
        color: farb.RGBToHSL(farb.unpack(color))[2] > 0.5 ? '#000' : '#fff'
      });

      // Change input value
      if (input.value != color) {
        input.value = color;

        // Update locked values
        if (propagate) {
          if ($(input).parent().next().is('.lock:not(.unlocked)')) {
            var matched = shift_color(color, reference[input.key], reference[input.key.replace(/1/, '2')]);
            callback(inputs[$(inputs).index(input) + 1], matched, false);
          }
          else if ($(input).parent().prev().is('.lock:not(.unlocked)')) {
            var matched = shift_color(color, reference[input.key], reference[input.key.replace(/2/, '1')]);
            callback(inputs[$(inputs).index(input) - 1], matched, false);
          }
        }

        // Reset colorscheme selector
        if (!colorscheme) {
          resetScheme();
        }
      }

    }

    /**
     * Reset the color scheme selector.
     */
    function resetScheme() {
      $('#edit-scheme', form).each(function () {
        this.selectedIndex = this.options.length - 1;
      });
    }

    // Focus the Farbtastic on a particular field.
    function focus() {
      var input = this;

      // Remove the transparent text if any.
      if (this.key == 'matte' && input.value == Drupal.settings.fivestar.transparent) {
        this.value = '';
      }

      // Remove old bindings
      focused && $(focused).unbind('keyup', farb.updateValue)
          .unbind('keyup', preview).unbind('keyup', resetScheme)
          .parent().removeClass('item-selected');

      // Add new bindings
      focused = this;
      farb.linkTo(function (color) { callback(input, color, true, false) });
      farb.setColor(this.value);
      $(focused).keyup(farb.updateValue).keyup(preview).keyup(resetScheme)
        .parent().addClass('item-selected');
    }

    function blur() {
      if (this.key == 'matte' && (!this.value || this.value == Drupal.settings.fivestar.transparent)) {
        this.value = Drupal.settings.fivestar.transparent;
        $(this).css({
          backgroundColor: '#fff',
          color: '#ccc'
        });
      }
      if (this.key != 'matte') {
        if (!this.value) {
          var rgb = this.style.backgroundColor.replace(/rgb\(([0-9, ]+)\)/, '$1').split(", ");
          rgb[0] = rgb[0] / 255;
          rgb[1] = rgb[1] / 255;
          rgb[2] = rgb[2] / 255;
          this.value = farb.pack(rgb);
        }
      }
    }

    // Initialize color fields
    $('#fivestar-palette input.form-text', form)
    .each(function () {
      // Extract palette field name
      this.key = this.id.substring(21);

      // Link to color picker temporarily to initialize.
      farb.linkTo(function () {}).setColor('#000').linkTo(this);

      // Add lock
      var i = inputs.length;
      if (this.name.match(/fivestar_colors\[[a-z]+1\]/)) {
        var lock = $('<div class="lock"></div>').click(function() { $(this).toggleClass('unlocked'); });
        $(this).parent().after(lock);
      }

      $(this).parent().find('.lock').click();
      this.i = i;
      inputs.push(this);
    })
    .focus(focus)
    .blur(blur);

    // Blur the matte color.
    blur.call(inputs[6]);

    // Focus first color
    focus.call(inputs[0]);

    // Hide secondary color fields and preview (called in toggleGradient).
    toggleGradient();
  });
}
