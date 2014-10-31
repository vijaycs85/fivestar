<?php

/**
 * @file
 * Contains \Drupal\fivestar\Plugin\Field\FieldFormatter\PercentageFormatter.
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
 *   id = "fivestar_percentage",
 *   label = @Translation("Percentage (i.e. 92)"),
 *   field_types = {
 *     "fivestar"
 *   },
 *   weight = 2
 * )
 */
class PercentageFormatter extends FivestarFormatterBase {

}
