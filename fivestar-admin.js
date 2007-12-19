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
    $title  = $('#edit-fivestar-title');
    $stars  = $('#edit-fivestar-stars');
    $style  = $('#edit-fivestar-style');
    $text   = $('#edit-fivestar-text');
    $labels = $('.fivestar-label input');
    $labels_enable = $('#edit-fivestar-labels-enable');

    // All the form elements except the enable checkbox.
    $options = $('#fivestar-node-type-form input:not(#edit-fivestar), #fivestar-node-type-form select');

    // Disable the settings if not enabled.
    if (!$enable.attr('checked')) {
      $options.attr('disabled', 'disabled');
    }

    // Add event handler for enable checkbox.
    $enable.change(function() {
      if ($(this).attr('checked')) {
        nodePreview.enable($unvote.attr('checked') ? 1 : 0, $stars.val(), $style.val(), $text.val(), $title.attr('checked') ? 1 : 0);

        if (commentPreview) {
          var commentSetting = 0;
          $comment.each(function() {
            if ($(this).attr('checked')) {
              commentSetting = this.value;
            }
          });
          if (commentSetting != 0) {
            commentPreview.enable(commentSetting == 1 ? 1 : 0, $stars.val(), 'user', 'none');
          }
        }
        $options.attr('disabled', false);
      }
      else {
        nodePreview.disable();
        if (commentPreview) {
          commentPreview.disable();
        }
        $options.attr('disabled', 'disabled');
      }
    });

    // Setup node preview handlers.
    $unvote.change(function() { nodePreview.setValue('unvote', $(this).attr('checked') ? 1 : 0); });
    $title.change(function() { nodePreview.setValue('title', $(this).attr('checked') ? 1 : 0); });
    $stars.change(function() {
      nodePreview.setValue('stars', this.value);
      nodePreview.displayTextfields();
    });
    $style.change(function() { nodePreview.setValue('style', this.value); });
    $text.change(function() { nodePreview.setValue('text', this.value); });

    // Handler for the star labels.
    var currentLabel = '';
    $labels.focus(function() {
      currentLabel = this.value;
    });
    $labels.blur(function() {
      if (currentLabel != this.value) {
        nodePreview.setLabel($labels.index(this), this.value);
      }
    });
    $labels_enable.change(function() { nodePreview.setValue('labels_enable', $(this).attr('checked') ? 1 : 0); });

    // Initialize the preview.
    if ($enable.attr('checked')) {
      var labels = new Array();
      $labels.each(function() {
        labels.push(this.value);
      });
      nodePreview.enable($unvote.attr('checked') ? 1 : 0, $stars.val(), $style.val(), $text.val(), $title.attr('checked') ? 1 : 0, labels);
    }

    // Hide extra mouseover textfields
    if ($enable.attr('checked')) {
      nodePreview.displayTextfields();
    }

    // Setup comment preview handlers and initialize.
    if (commentPreview) {
      // Setup comment preview handlers.
      $stars.change(function() { commentPreview.setValue('stars', this.value); });
      $comment.change(function() {
        if ($(this).attr('checked') && $enable.attr('checked')) {
          if (this.value != 0) {
            commentPreview.setValue('unvote', this.value == 1 ? 1 : 0);
            commentPreview.enable(this.value == 1 ? 1 : 0, $stars.val(), 'user', 'none', 0);
          }
          else {
            commentPreview.disable();
          }
        }
      });

      $labels.blur(function() {
        if (currentLabel != this.value) {
          commentPreview.setLabel($labels.index(this), this.value);
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
        var labels = new Array();
        $labels.each(function() {
          labels.push(this.value);
        });
        commentPreview.enable(commentSetting == 1 ? 1 : 0, $stars.val(), 'user', 'none', 0, labels);
      }
      $labels_enable.change(function() { commentPreview.setValue('labels_enable', $(this).attr('checked') ? 1 : 0); });
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
  this.title = 1;
  this.stars = 5;
  this.style = '';
  this.text = '';
  this.labels = new Array();
  this.labels_enable = false;
};

/**
 * Enable the preview functionality and show the preview.
 */
fivestarPreview.prototype.enable = function(unvote, stars, style, text, title, labels, labels_enable) {
  if (!this.enabled) {
    this.enabled = true;
    this.unvote = unvote;
    this.title = title;
    this.stars = stars;
    this.style = style;
    this.text = text;
    this.labels = labels;
    this.labels_enable = labels_enable;
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

fivestarPreview.prototype.setLabel = function(delta, value) {
  this.labels[delta] = value;
  if (this.enabled) {
    this.update();
  }
}

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
      $('div.fivestar-form-item', self.preview).rating();
      $('input.fivestar-submit', self.preview).hide();
      $(self.preview).show();
    };

    // Prepare data to send to the server.
    var data = { style: this.style, text: this.text, stars: this.stars, unvote: this.unvote, title: this.title, labels_enable: this.labels_enable }

    // Covert labels array format understood by PHP and add to data.
    for (n in this.labels) {
      data['labels['+ n +']'] = this.labels[n];
    }

    $.ajax({
      dateType: 'json',
      type: 'POST',
      url: Drupal.settings.fivestar.preview_url,
      data: data,
      success: updateSuccess,
    });
  }
};

// Display the appropriate number of text fields for the mouseover star descriptions
fivestarPreview.prototype.displayTextfields = function() {
  if (this.enabled) {
    for (var count = 0; count < 10; count++) {
      if (count < this.stars) {
        $('#fivestar-label-'+ count).show();
      }
      else {
        $('#fivestar-label-'+count).css('display', 'none');
      }
    }
  }
};
