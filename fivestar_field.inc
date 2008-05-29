<?php
// $Id$

/**
 * @file
 * Provides CCK integration for fivestar module
 */

/**
 * Implementation of hook_field_info().
 */
function fivestar_field_info() {
  return array(
    'fivestar' => array(
      'label' => t('Fivestar Rating'),
      'description' => t('Store a rating for this piece of content.'),
    ),
  );
}

/**
 * Implementation of hook_field_settings().
 */
function fivestar_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      // Multiple is not supported with Fivestar.
      $form['multiple'] = array(
        '#type' => 'value',
        '#value' => 0,
      );
      $form['stars'] = array(
        '#type' => 'select',
        '#title' => t('Number of Options'),
        '#options' => drupal_map_assoc(range(1, 10)),
        '#default_value' => $field['stars'] ? $field['stars'] : 5,
        '#description' => t('The number of stars or radio buttons to display.'),
      );

      if (module_exists('nodecomment')) {
        $form['dynamic_target'] = array(
          '#title' => t('Use Node Comment Parent as Target'),
          '#type' => 'checkbox',
          '#default_value' => $field['dynamic_target'],
          '#return_value' => 'comment_target_nid',
          '#description' => t('Use this option to easily make a Fivestar field affect the comment parent for nodes of this content type.')
        );
      }

      $form['target'] = array(
        '#type' => 'textarea',
        '#title' => t('Target Node ID'),
        '#default_value' => $field['target'] ? $field['target'] : '',
        '#description' => t(
          'A single node ID on which this field will register the vote. If no NID is specified, the field\'s value will be saved, but no vote will be registered with the Voting API.'
        ),
      );

      if (user_access('use PHP for fivestar target')) {
        $form['target']['#description'] .= ' '. t(
          ' Return the target node ID or use plain text. Enter PHP code between <em>&lt;?php ?&gt;</em>. Note that executing incorrect PHP-code can break your Drupal site.'
        );
        
        $form['php'] = array(
          '#type' => 'value',
          '#value' => 1,
        );
      }

      $form['axis'] = array(
        '#type' => 'textfield',
        '#title' => 'Voting Axis',
        '#description' => t('The axis this rating will affect. Enter a property on which that this rating will affect, such as <em>quality</em>, <em>satisfaction</em>, <em>overall</em>, etc. If no axis is entered, the default axis <em>vote</em> will be used. Warning: changing this value will not update existing votes to the new axis.'),
        '#default_value' => $field['axis'],
      );

      return $form;
    case 'save':
      return array('stars', 'dynamic_target', 'target', 'php', 'axis');
    case 'database columns':
      return array(
        'target' => array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE),
        'rating' => array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE),
      );
  }
}
  
/**
 * Implementation of hook_field().
 */
function fivestar_field($op, &$node, $field, &$items, $teaser, $page) {
  $fieldname = $field['field_name'];

  switch ($op) {
    case 'insert':
    case 'update':
      foreach ($items as $delta => $item) {
        if ($field['dynamic_target'] && !empty($node->$field['dynamic_target'])) {
          $items[$delta]['target'] = $node->$field['dynamic_target'];
        }
        elseif (is_numeric($item['target'])) {
          $items[$delta]['target'] = $item['target'];
        }
        elseif ($field['php'] && strpos($item['target'], '<?php') === 0) {
          // Use eval rather than drupal_eval to allow access to local variables.
          $items[$delta]['target'] = eval('?>'. $item['target']);
        }
        if (is_numeric($items[$delta]['target'])) {
          _fivestar_cast_vote('node', $items[$delta]['target'], $item['rating'], $item['axis'], $node->uid);
          votingapi_recalculate_results('node', $items[$delta]['target']);
        }
      }
      $items = array($item);
      break;
    case 'delete':
      foreach ($items as $delta => $item) {
        if ($field['dynamic_target'] && !empty($node->$field['dynamic_target'])) {
          $items[$delta]['target'] = $node->$field['dynamic_target'];
        }
        elseif (is_numeric($item['target'])) {
          $items[$delta]['target'] = $item['target'];
        }
        elseif ($field['php'] && strpos($item['target'], '<?php') === 0) {
          // Use eval rather than drupal_eval to allow access to local variables.
          $items[$delta]['target'] = eval('?>'. $item['target']);
        }
        if (is_numeric($item['target'])) {
          _fivestar_cast_vote('node', $item['target'], 0, $item['axis'], $node->uid);
          votingapi_recalculate_results('node', $item['target']);
        }
      }
      break;
    case 'sanitize':
      $items[0]['stars'] = $field['stars'];
      break;
  }
}
      
/**
 * Implementation of hook_widget_info().
 */
function fivestar_widget_info() {
  return array(
    'radios' => array(
      'label' => t('Radio Buttons'),
      'field types' => array('fivestar'),
      'multiple values' => CONTENT_HANDLE_MODULE,
    ),
    'stars' => array(
      'label' => t('Stars'),
      'field types' => array('fivestar'),
      'multiple values' => CONTENT_HANDLE_MODULE,
    ),
  );
}

/**
 * Implementation of hook_widget_settings().
 */
function fivestar_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      $form = array();
      $form['allow_clear'] = array(
        '#type' => 'select',
        '#title' => t('Allow Clearing'),
        '#options' => array(1 => t('True'), 0 => t('False')),
        '#default_value' => $widget['allow_clear'] ? $widget['allow_clear'] : 1,
        '#description' => t("Display a button to clear the user's current vote."),
      );
      return $form;
    case 'save':
      return array('allow_clear');
  }
}

/**
 * Implementation of hook_widget().
 */
function fivestar_widget(&$form, &$form_state, $field, $items) {
  $element = array('#tree' => TRUE);
  $element['rating'] = array(
    '#type' => 'fivestar',
    '#title' => $field['widget']['label'],
    '#default_value' => isset($items[0]['rating']) ? $items[0]['rating'] : NULL,
    '#stars' => is_numeric($field['stars']) ? $field['stars'] : 5,
    '#allow_clear' => $field['widget']['allow_clear'],
    '#description' => $field['widget']['description'],
    '#weight' => $field['widget']['weight'],
    '#auto_submit' => FALSE,
    '#widget' => $field['widget']['type'],
    '#required' => $field['required'],
    '#labels_enable' => FALSE,
  );

  $element['target'] = array(
    '#type' => 'value',
    '#value' => $field['target'],
  );
  $element['axis'] = array(
    '#type' => 'value',
    '#value' => $field['axis'],
  );

  // CCK likes to always have a 2D array for form elements.
  $element = array($element);

  return $element;
}
      
/**
 * Implementation of hook_field_formatter_info().
 */
function fivestar_field_formatter_info() {
  return array(
    'default' => array(
      'label' => t('As Stars'),
      'field types' => array('fivestar'),
    ),
    'rating' => array(
      'label' => t('Rating (i.e. 4.2/5)'),
      'field types' => array('fivestar'),
    ),
    'percentage' => array(
      'label' => t('Percentage (i.e. 92)'),
      'field types' => array('fivestar'),
    ),
  );
}

/**
 * Implementation of hook_field_formatter().
 *
 * The $node argument is necessary so that filter access can be checked on
 * node preview.
 */
function fivestar_field_formatter($field, $item, $formatter, $node) {
  if (!isset($item['rating'])) {
    $item['rating'] = 0;
  }

  switch ($formatter) {
    case 'rating':
      return round(100/$item['rating'], 1)/$field['stars'];

    case 'percentage':
      return $item['rating'];

    default:
      return theme('fivestar_static', $item['rating'], $field['stars']);
  }
}

/**
 * Theme function for 'default' fivestar field formatter.
 */
function theme_fivestar_formatter_default($element) {
  if (!isset($element['#item']['rating'])) {
    $element['#item']['rating'] = 0;
  }

  return theme('fivestar_static', $element['#item']['rating'], $element['#item']['stars']);
}

/**
 * Theme function for 'default' fivestar field formatter.
 */
function theme_fivestar_formatter_rating($element) {
  if (!isset($element['#item']['rating'])) {
    $element['#item']['rating'] = 0;
  }

  return theme('fivestar_static', $element['#item']['rating'], $element['#field']['stars']);
}

/**
 * Theme function for 'default' fivestar field formatter.
 */
function theme_fivestar_formatter_percentage($element) {
  if (!isset($element['#item']['rating'])) {
    $element['#item']['rating'] = 0;
  }

  return round(100/$element['#item']['rating'], 1)/$element['#field']['stars'];
}
