<?php

/**
 * @file
 * Contains \Drupal\fivestar\Plugin\Field\FieldWidget\Stars.
 */

namespace Drupal\fivestar\Plugin\Field\FieldWidget;

use Drupal\Component\Utility\String;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'fivestar_stars' widget.
 *
 * @FieldWidget(
 *   id = "fivestar_stars",
 *   label = @Translation("Stars (rated while editing)"),
 *   field_types = {
 *     "fivestar"
 *   }
 * )
 */
class Stars extends FiveStartWidgetBase {
  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return array(
      'fivestar_widget' => '',
    ) + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $elements = parent::settingsForm($form, $form_state);
    $elements['widget'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Star display options'),
      '#description' => t('Choose a style for your widget.'),
      '#weight' => -2,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $default = $this->getSetting('fivestar_widget') ?: 'default';

    $elements['widget']['fivestar_widget'] = array(
      '#type' => 'radios',
      '#options' => array('default' => t('Default')) + $this->getAllWidget(),
      '#default_value' => $default,
      '#attributes' => array('class' => array('fivestar-widgets', 'clearfix')),
      '#pre_render' => array('fivestar_previews_expand'),
      '#attached' => array('css' => array(drupal_get_path('module', 'fivestar') . '/css/fivestar-admin.css')),
    );

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $i18n = function_exists('i18n_field_translate_property');
    if (isset($form['#title']) && $form['#title'] == 'Default value') {
      $options = array(0 => t('No stars'));
      $star_settings = $this->getSetting('stars');
      if (empty($star_settings)) {
        $instance['settings']['stars'] = 5;
      }
      for ($i = 1; $i <= $star_settings; $i++) {
        $percentage = ceil($i * 100 / $star_settings);
        $options[$percentage] = $this->getStringTranslation()->formatPlural($i, '1 star', '@count stars');
      }
      $elements['rating'] = array(
        '#type' => 'select',
        '#title' => $this->t(String::checkPlain($this->fieldDefinition->getLabel())),
        '#options' => $options,
        '#default_value' => isset($items[$delta]['rating']) ? $items[$delta]['rating'] : NULL,
        '#description' => $this->t(String::checkPlain($this->fieldDefinition->getDescription())),
        '#required' => isset($instance['required']) ? $instance['required'] : FALSE,
      );

    }
    else {
      $widgets = $this->getAllWidget();
      $active = $this->getSetting('fivestar_widget') ?: 'default';
      $widget = array(
        'name' => isset($widgets[$active]) ? strtolower($widgets[$active]) : 'default',
        'css' => $active,
      );

      $values = array(
        'user' => 0,
        'average' => 0,
        'count' => 0,
      );

      $settings = array(
        'stars' => $this->getSetting('stars'),
        'allow_clear' => $this->getSetting('allow_clear') ?: FALSE,
        'allow_revote' => $this->getSetting('allow_revote') ?: FALSE,
        'allow_ownvote' => $this->getSetting('allow_ownvote') ?: FALSE,
        'style' => 'user',
        'text' => 'none',
        'widget' => $widget,
      );

      $element['rating'] = array(
        '#type' => 'fivestar',
        '#title' => isset($instance['label']) ? (($i18n) ? i18n_field_translate_property($instance, 'label') : t($instance['label'])) : FALSE,
        '#stars' => $this->getSetting('stars') ?: 5,
        '#allow_clear' => $this->getSetting('allow_clear') ?: FALSE,
        '#allow_revote' => $this->getSetting('allow_revote') ?: FALSE,
        '#allow_ownvote' => $this->getSetting('allow_ownvote') ?: FALSE,
        '#default_value' => isset($items[$delta]['rating']) ? $items[$delta]['rating'] : (isset($instance['default_value'][$delta]['rating']) ? $instance['default_value'][$delta]['rating'] : 0),
        '#widget' => $widget,
        '#settings' => $settings,
        '#values' => $values,
        '#description' => isset($instance['description']) ? (($i18n) ? i18n_field_translate_property($instance, 'description') : t($instance['description'])) : FALSE,
        '#required' => isset($instance['required']) ? $instance['required'] : FALSE,
      );
    }
    return $elements;
  }

  /**
   * @return array
   */
  protected function getAllWidget() {
    return \Drupal::moduleHandler()->invokeAll('fivestar_widgets');
  }

}
