<?php

/**
 * @file
 * Contains \Drupal\fivestar\Plugin\Field\FieldFormatter\DefaultFormatter.
 */

namespace Drupal\fivestar\Plugin\Field\FieldFormatter;

use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'number_decimal' formatter.
 *
 * The 'Default' formatter is different for integer fields on the one hand, and
 * for decimal and float fields on the other hand, in order to be able to use
 * different settings.
 *
 * @FieldFormatter(
 *   id = "fivestar_default",
 *   label = @Translation("Default"),
 *   field_types = {
 *     "fivestar"
 *   },
 *   weight = 1
 * )
 */
class DefaultFormatter extends FivestarFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return array(
      'widget' => array('fivestar_widget' => NULL),
      'style' => 'average',
      'text' => 'average',
      'expose' => TRUE,
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
      '#title' => $this->t('Star display options'),
      '#description' => $this->t('Choose a style for your widget.'),
      '#weight' => -2,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $widgets = $this->getAllFivestarWidgets();
    $elements['widget']['fivestar_widget'] = array(
      '#type' => 'radios',
      '#options' => array('default' => $this->t('Default')) + $widgets,
      '#default_value' => isset($settings['widget']['fivestar_widget']) ? $settings['widget']['fivestar_widget'] : 'default',
      '#attributes' => array('class' => array('fivestar-widgets', 'clearfix')),
      '#pre_render' => array('fivestar_previews_expand'),
      '#attached' => array('css' => array(drupal_get_path('module', 'fivestar') . '/css/fivestar-admin.css')),
    );

    //if ($this-> == 'exposed') {
    $elements['expose'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t('Allow voting on the entity.'),
      '#default_value' => $settings['expose'],
      '#return_value' => 1
    );
    // }

    $elements['style'] = array(
      '#type' => 'select',
      '#title' => $this->t('Value to display as stars'),
      '#default_value' => $settings['style'],
      '#options' => array(
        'average' => $this->t('Average vote'),
        'user' => $this->t("User's vote"),
        'smart' => $this->t("User's vote if available, average otherwise"),
        'dual' => $this->t("Both user's and average vote"),
      ),
    );
    $elements['text'] = array(
      '#type' => 'select',
      '#title' => $this->t('Text to display under the stars'),
      '#default_value' => $settings['text'],
      '#options' => array(
        'none' => $this->t('No text'),
        'average' => $this->t('Average vote'),
        'user' => $this->t("User's vote"),
        'smart' => $this->t("User's vote if available, average otherwise"),
        'dual' => $this->t("Both user's and average vote"),
      ),
    );

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = array();
    //$this->getAllFivestarWidgets();
    $settings = $this->getSettings();
    $widget = $settings['widget'];
    $this->getPluginDefinition();
    // FIXME Vijay
    // if ($instance['widget']['type'] == 'exposed') {
    if (0) {
      $summary[] = t("Style: @widget, Exposed: @expose, Stars display: @style, Text display: @text", array(
        '@widget' => isset($widgets[$widget]) ? strtolower($widgets[$widget]) : t('default'),
        '@expose' => ($settings['expose']) ? 'yes' : 'no',
        '@style' => strtolower($settings['style']),
        '@text' => strtolower($settings['text'])));
      return $summary;
    }

    $summary[] = t("Style: @widget, Stars display: @style, Text display: @text", array(
      '@widget' => isset($widgets[$widget]) ? strtolower($widgets[$widget]) : t('default'),
      '@style' => strtolower($settings['style']),
      '@text' => strtolower($settings['text'])));

    return $summary;
  }

}
