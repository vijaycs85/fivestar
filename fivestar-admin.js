// $Id$

/**
 * Fivestar admin interface enhancments.
 */

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var nodePreview = new fivestarPreview($('#fivestar-direct-preview .fivestar-preview')[0]);

    // Enable comments if available.
    $comment = $('input[@name=fivestar_comment]');
    if ($comment.size()) {
      var commentPreview = new fivestarPreview($('#fivestar-comment-preview .fivestar-preview')[0]);
    }

    // Setup dynamic form elements.
    $enable = $('#edit-fivestar');
    $unvote = $('#edit-fivestar-unvote');
    $stars = $('#edit-fivestar-stars');
    $style = $('#edit-fivestar-style');

    // Add event handler for enable checkbox.
    $enable.change(function() {
      if ($(this).attr('checked')) {
        nodePreview.enable($unvote.attr('checked') ? 1 : 0, $stars.val(), $style.val());

        if (commentPreview) {
          var commentSetting = 0;
          $comment.each(function() {
            if ($(this).attr('checked')) {
              commentSetting = this.value;
            }
          });
          if (commentSetting != 0) {
            commentPreview.enable(commentSetting == 1 ? 1 : 0, $stars.val(), 'compact');
          }
        }
      }
      else {
        nodePreview.disable();
        if (commentPreview) {
          commentPreview.disable();
        }
      }
    });

    // Setup node preview handlers.
    $unvote.change(function() { nodePreview.setValue('unvote', $(this).attr('checked') ? 1 : 0); });
    $stars.change(function() { nodePreview.setValue('stars', this.value); });
    $style.change(function() { nodePreview.setValue('style', this.value); });
    // Initialize the preview.
    if ($enable.attr('checked')) {
      nodePreview.enable($unvote.attr('checked') ? 1 : 0, $stars.val(), $style.val());
    }

    // Setup comment preview handlers and initialize.
    if (commentPreview) {
      // Setup comment preview handlers.
      $stars.change(function() { commentPreview.setValue('stars', this.value); });
      $comment.change(function() {
        if ($(this).attr('checked') && $enable.attr('checked')) {
          if (this.value != 0) {
            commentPreview.setValue('unvote', this.value == 1 ? 1 : 0);
            commentPreview.enable(this.value == 1 ? 1 : 0, $stars.val(), 'compact');
          }
          else {
            commentPreview.disable();
          }
        }
      });

      // Setup comment
      var commentSetting = 0;
      $comment.each(function() {
        if ($(this).attr('checked')) {
          commentSetting = this.value;
        }
      });
      if ($enable.attr('checked') && commentSetting > 0) {
        commentPreview.enable(commentSetting == 1 ? 1 : 0, $stars.val(), 'compact');
      }
    }
  });
}

/**
 * Constructor for fivestarPreview.
 * @param previewId
 *   The id attribute of the div containing the preview.
 */
var fivestarPreview = function(previewElement) {
  this.preview = previewElement;
  this.enabled = false;
  this.unvote = 0;
  this.stars = 5;
  this.style = '';
};

/**
 * Enable the preview functionality and show the preview.
 */
fivestarPreview.prototype.enable = function(unvote, stars, style) {
  if (!this.enabled) {
    this.enabled = true;
    this.unvote = unvote;
    this.stars = stars;
    this.style = style;
    $(this.preview).show();
    this.update();
  }
};

/**
 * Disable the preview functionality and show the preview.
 */
fivestarPreview.prototype.disable = function() {
  if (this.enabled) {
    this.enabled = false;
    $(this.preview).hide();
  }
};

fivestarPreview.prototype.setValue = function(field, value) {
  if (this[field] != value) {
    this[field] = value;
    if (this.enabled) {
      this.update();
    }
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
      $(self.preview).html(response.data).hide();
      $('div.fivestar-widget', self.preview).rating();
      $('input.fivestar-submit', self.preview).hide();
      $(self.preview).show();
    };

    $.ajax({
      dateType: 'json',
      url: Drupal.settings.fivestar.preview_url + '/node/' + this.style + '/' + this.stars + '/' + this.unvote,
      success: updateSuccess,
    });
  }
};
