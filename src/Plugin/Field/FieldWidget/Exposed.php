<?php

/**
 * @file
 * Contains \Drupal\fivestar\Plugin\Field\FieldWidget\Exposed.
 */

namespace Drupal\fivestar\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'fivestar_exposed' widget.
 *
 * @FieldWidget(
 *   id = "fivestar_exposed",
 *   label = @Translation("Stars (rated while viewing)"),
 *   field_types = {
 *     "fivestar"
 *   }
 * )
 */
class Exposed extends FiveStartWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    return $element;
  }

}
