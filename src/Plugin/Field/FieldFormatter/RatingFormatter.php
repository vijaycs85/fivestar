<?php

/**
 * @file
 * Contains \Drupal\fivestar\Plugin\Field\FieldFormatter\RatingFormatter.
 */

namespace Drupal\fivestar\Plugin\Field\FieldFormatter;

/**
 * Plugin implementation of the 'number_decimal' formatter.
 *
 * The 'Default' formatter is different for integer fields on the one hand, and
 * for decimal and float fields on the other hand, in order to be able to use
 * different settings.
 *
 * @FieldFormatter(
 *   id = "fivestar_rating",
 *   label = @Translation("Rating (i.e. 4.2/5)"),
 *   field_types = {
 *     "fivestar"
 *   },
 *   weight = 3
 * )
 */
class RatingFormatter extends FivestarFormatterBase {

}
