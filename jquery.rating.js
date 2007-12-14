/**
 * Modified Star Rating - jQuery plugin
 *
 * Copyright (c) 2006 Wil Stuckey
 *
 * Original source available: http://sandbox.wilstuckey.com/jquery-ratings/
 * Extensively modified by Lullabot: http://www.lullabot.com
 *
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 */

/**
 * Create a degradeable star rating interface out of a simple form structure.
 * Returns a modified jQuery object containing the new interface.
 *   
 * @example jQuery('form.rating').rating();
 * @cat plugin
 * @type jQuery 
 *
 */
(function($){ // Create local scope.
    /**
     * Takes the form element, builds the rating interface and attaches the proper events.
     * @param {Object} $obj
     */
    var buildRating = function($obj){
        var $widget = buildInterface($obj),
            $stars = $('.star', $widget),
            $cancel = $('.cancel', $widget),
            averageIndex = $("select", $obj).val();
            averagePercent = 0;

        // Record star display.
        if ($obj.is('.fivestar-user-stars')) {
          var starDisplay = 'user';
        }
        else if ($obj.is('.fivestar-average-stars')) {
          var starDisplay = 'average';
        }
        else if ($obj.is('.fivestar-combo-stars')) {
          var starDisplay = 'combo';
        }
        else {
          var starDisplay = 'none';
        }
        // Recore text display.
        if ($obj.is('.fivestar-user-text')) {
          var textDisplay = 'user';
        }
        else if ($obj.is('.fivestar-average-text')) {
          var textDisplay = 'average';
        }
        else if ($obj.is('.fivestar-combo-text')) {
          var textDisplay = 'combo';
        }
        else {
          var textDisplay = 'none';
        }

        // Add hover and focus events.
        $stars
            .mouseover(function(){
                event.drain();
                event.fill(this);
            })
            .mouseout(function(){
                event.drain();
                event.reset();
            })
            .focus(function(){
                event.drain();
                event.fill(this)
            })
            .blur(function(){
                event.drain();
                event.reset();
            });
        
        // Cancel button events.
        $cancel
            .mouseover(function(){
                event.drain();
                $(this).addClass('on')
            })
            .mouseout(function(){
                event.reset();
                $(this).removeClass('on')
            })
            .focus(function(){
                event.drain();
                $(this).addClass('on')
            })
            .blur(function(){
                event.reset();
                $(this).removeClass('on')
            });
        
        // Click events.
        $cancel.click(function(){
            event.drain();
            averageIndex = 0;
            averagePercent = 0;
            // Save the value in a hidden field.
            $("select", $obj).val(averageIndex);
            // Submit the form if needed.
            $("input.fivestar-path", $obj).each(function () { $.ajax({ type: 'GET', dataType: 'xml', url: this.value + '/' + averageIndex, success: voteHook }); });
            return false;
        });
        $stars.click(function(){
            averageIndex = Math.ceil(($stars.index(this) + 1) * (100/$stars.size()));
            averagePercent = 0;
            // Save the value in a hidden field.
            $("select", $obj).val(averageIndex);
            // Submit the form if needed.
            $("input.fivestar-path", $obj).each(function () { $.ajax({ type: 'GET', dataType: 'xml', url: this.value + '/' + averageIndex, success: voteHook }); });
            return false;
        });

        var event = {
            fill: function(el){ // Fill to the current mouse position.
                var index = $stars.index(el) + 1;
                $stars
                    .children('a').css('width', '100%').end()
                    .filter(':lt(' + index + ')').addClass('hover').end();
            },
            drain: function() { // Drain all the stars.
                $stars
                    .filter('.on').removeClass('on').end()
                    .filter('.hover').removeClass('hover').end();
            },
            reset: function(){ // Reset the stars to the default index.
                $stars.filter(':lt(' + Math.floor(averageIndex/100 * $stars.size()) + ')').addClass('on').end();
                var percent = (averagePercent) ? averagePercent * 10 : 0;
                if (percent > 0) {
                    $stars.eq(averageIndex).addClass('on').children('a').css('width', percent + "%").end().end();
                }
            }
        };

        /**
         * Checks for the presence of a javascript hook 'fivestarResult' to be
         * called upon completion of a AJAX vote request.
         */
        var voteHook = function(data) {
          var returnObj = {
            result: {
              count: $("result > count", data).text(),
              average: $("result > average", data).text(),
              summary: { average: $("summary average", data).text(), user: $("summary user", data).text(), combo: $("summary combo", data).text() }
            },
            vote: {
              id: $("vote id", data).text(),
              type: $("vote type", data).text(),
              value: $("vote value", data).text()
            },
            display: {
              stars: starDisplay,
              text: textDisplay
            }
          };
          // Check for a custom callback.
          if (window.fivestarResult) {
            fivestarResult(returnObj);
          }
          // Use the default.
          else {
            fivestarDefaultResult(returnObj);
          }
        };

        event.reset();
        return $widget;
    };
    
    /**
     * Accepts jQuery object containing a single fivestar widget.
     * Returns the proper div structure for the star interface.
     * 
     * @return jQuery
     * @param {Object} $widget
     * 
     */
    var buildInterface = function($widget){
        var $container = $('<div class="fivestar-widget clear-block"></div>');
        var $options = $("select option", $widget);
        var size = $('option', $widget).size() - 1;
        var cancel = 1;
        for (var i = 0, option; option = $options[i]; i++){
            if (option.value == "0") {
              cancel = 0;
              $div = $('<div class="cancel"><a href="#0" title="' + option.text + '">' + option.text + '</a></div>');
            }
            else {
              var zebra = (i + cancel) % 2 == 0 ? 'even' : 'odd';
              var count = i + cancel;
              $div = $('<div class="star star-' + count + ' star-' + zebra + '"><a href="#' + option.value + '" title="' + option.text + '">' + option.text + '</a></div>');
            }
            $container.append($div[0]);                    
        }
        $container.addClass('fivestar-widget-' + (size + cancel));
        // Attach the new widget and hide the existing widget.
        $widget.after($container).hide();
        return $container;
    };

    /**
     * Standard handler to update the average rating when a user changes their
     * vote. This behavior can be overridden by implementing a fivestarResult
     * function in your own module or theme.
     * @param object voteResult
     * Object containing the following properties from the vote result:
     * voteResult.result.count The current number of votes for this item.
     * voteResult.result.average The current average of all votes for this item.
     * voteResult.result.summary.average The textual description of the average.
     * voteResult.result.summary.user The textual description of the user's current vote.
     * voteResult.vote.id The id of the item the vote was placed on (such as the nid)
     * voteResult.vote.type The type of the item the vote was placed on (such as 'node')
     * voteResult.vote.value The value of the new vote saved
     * voteResult.display.stars The type of star display we're using. Either 'average', 'user', or 'combo'.
     * voteResult.display.text The type of text display we're using. Either 'average', 'user', or 'combo'.
     */
    function fivestarDefaultResult(voteResult) {
      $('div.fivestar-summary-'+voteResult.vote.id).html(voteResult.result.summary[voteResult.display.text]);
    };

    /**
     * Set up the plugin
     */
    $.fn.rating = function() {
      var stack = [];
      this.each(function() {
          var ret = buildRating($(this));
          stack.push(ret);
      });
      return stack;
    };

  // Fix ie6 background flicker problem.
  if ($.browser.msie == true) {
    try {
      document.execCommand('BackgroundImageCache', false, true);
    } catch(err) {}
  }
})(jQuery);

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    $('div.fivestar-form-item').rating();
    $('input.fivestar-submit').hide();
  });
}