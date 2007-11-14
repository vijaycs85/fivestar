// $Id$

/**
 * Fivestar admin interface enhancments.
 */

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var preview = new fivestarPreview('fivestar-preview');

    // Setup dynamic form elements.
    $enable = $('#edit-fivestar');
    $unvote = $('#edit-fivestar-unvote');
    $stars = $('#edit-fivestar-stars');
    $style = $('#edit-fivestar-style');

    // Add event handlers.
    $enable.change(function() {
      if ($(this).attr('checked')) {
        preview.enable($unvote.attr('checked') ? 1 : 0, $stars.val(), $style.val());
        preview.update();
      }
      else {
        preview.disable();
      }
    });
    $unvote.change(function() { preview.setValue('unvote', $(this).attr('checked') ? 1 : 0); });
    $stars.change(function() { preview.setValue('stars', this.value); });
    $style.change(function() { preview.setValue('style', this.value); });

    // Initialize the preview.
    preview.enable($unvote.attr('checked') ? 1 : 0, $stars.val(), $style.val());
  });
}

/**
 * Constructor for fivestarPreview.
 * @param previewId
 *   The id attribute of the div containing the preview.
 */
var fivestarPreview = function(previewId) {
  this.preview = $('#' + previewId);
  this.enabled = true;
  this.unvote = 0;
  this.stars = 5;
  this.style = '';
};

/**
 * Enable the preview functionality and show the preview.
 */
fivestarPreview.prototype.enable = function(unvote, stars, style) {
  this.enabled = true;
  this.unvote = unvote;
  this.stars = stars;
  this.style = style;
  $(this.preview).show();
};

/**
 * Disable the preview functionality and show the preview.
 */
fivestarPreview.prototype.disable = function() {
  this.enabled = false;
  $(this.preview).hide();
};

fivestarPreview.prototype.setValue = function(field, value) {
  if (this[field] != value) {
    this[field] = value;
    this.update();
  }
};

fivestarPreview.prototype.update = function() {
  if (this.enabled) {
    var self = this;
    var updateSuccess = function(response) {
      // Sanity check for browser support (object expected).
      // When using iFrame uploads, responses must be returned as a string.
      if (typeof(response) == 'string') {
        response = Drupal.parseJson(response);
      }
      $(self.preview).html(response.data);
      $('div.fivestar-widget', self.preview).rating();
      $('input.fivestar-submit', self.preview).hide();
    };

    $.ajax({
      dateType: 'json',
      url: Drupal.settings.fivestar.preview_url + '/node/' + this.style + '/' + this.stars + '/' + this.unvote,
      success: updateSuccess,
    });
  }
};
