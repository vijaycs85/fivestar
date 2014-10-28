<?php

/**
 * @file
 * Contains \Drupal\fivestar\Plugin\Field\FieldType\FivestarItem.
 */

namespace Drupal\fivestar\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldItemBase;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\Core\TypedData\DataDefinition;

/**
 * Plugin implementation of the 'telephone' field type.
 *
 * @FieldType(
 *   id = "fivestar",
 *   label = @Translation("Fivestar rating"),
 *   description = @Translation("This field stores a fivestar rating in the database."),
 *   default_widget = "fivestar_select",
 *   default_formatter = "fivestar_formatter_default"
 * )
 */
class FivestarItem extends FieldItemBase {
  /**
   * Definitions of the contained properties.
   *
   * @var array
   */
  static $propertyDefinitions;
  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    return array(
      'columns' => array(
        'rating' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => FALSE,
          'sortable' => TRUE
        ),
        'target' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => FALSE
        ),
      ),
    );
  }
  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    $property_definitions['rating'] = DataDefinition::create('integer')
      ->setLabel(t('Rating'));
    $property_definitions['target'] = DataDefinition::create('integer');
    return $property_definitions;
  }

}
