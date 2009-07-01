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
    'fivestar' => array('label' => t('Fivestar')),
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
        '#title' => $field['widget']['type'] == 'stars' ? t('Number of Stars') : t('Number of Options'),
        '#options' => drupal_map_assoc(range(1, 10)),
        '#default_value' => $field['stars'] ? $field['stars'] : 5,
      );

      $dynamic_options = array();
      if (module_exists('nodecomment')) {
        $dynamic_options['comment_target_nid'] = t('Node Comment Parent');
      }
      if (module_exists('nodereference')) {
        $fields = content_fields();
        foreach ($fields as $field_name => $content_field) {
          // Make sure that this field exists for this type.
          if ($content_field['type'] == 'nodereference' && $content_field = content_fields($field_name, $field['type_name'])) {
            $dynamic_options[$content_field['field_name']] = t('Node reference: @field', array('@field' => $content_field['field_name']));
          }
        }
      }

      if (empty($dynamic_options)) {
        drupal_set_message(t('No potential target fields are available for the %type type. Create a node reference field in this node type to make it easier to assign a vote to a node.', array('%type' => node_get_types('name', $field['type_name']))), 'warning');
      }

      $dynamic_options = array('' => '<'. t('none') .'>') + $dynamic_options;
      $form['dynamic_target'] = array(
        '#title' => t('Voting target'),
        '#type' => 'select',
        '#default_value' => $field['dynamic_target'],
        '#options' => $dynamic_options,
        '#description' => t('The voting target will make the value of this field cast a vote on another node. Use node reference fields (part of CCK core) or <a href="http://drupal.org/project/nodecomment">Node Comments</a> module to create advanced reviews. More information available on the <a href="http://drupal.org/handbook/modules/fivestar">Fivestar handbook page</a>.')
      );

      if (user_access('use PHP for fivestar target')) {
        $form['php_target'] = array(
          '#type' => 'fieldset',
          '#title' => t('Voting target PHP code'),
          '#collapsible' => TRUE,
          '#collapsed' => empty($field['php_target']),
        );

        $form['php_target']['php_target'] = array(
          '#title' => t('Code'),
          '#type' => 'textarea',
          '#default_value' => $field['php_target'] ? $field['php_target'] : '',
          '#description' => t('Advanced usage only: PHP code that returns a target node ID. Should not include &lt;?php ?&gt; delimiters. If this field is filled out, the value returned by this code will override any value specified above. Note that executing incorrect PHP-code can break your Drupal site.'),
        );
      }
      else {
        $form['php_target'] = array(
          '#type' => 'value',
          '#value' => $field['php_target'] ? $field['php_target'] : '',
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
      return array('stars', 'dynamic_target', 'php_target', 'axis');
    case 'database columns':
      $columns = array(
        'rating' => array('type' => 'int', 'default' => 'NULL', 'sortable' => TRUE),
        'target' => array('type' => 'int', 'default' => 'NULL'),
      );
      return $columns;
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
    case 'delete':
      foreach ($items as $delta => $item) {
        if ($node->status == 0 || $op == 'delete') {
          $rating = 0;
        } else {
          $rating = $items[$delta]['rating'];
        }
        $items[$delta]['target'] = fivestar_field_target($node, $field, $item);

        if (is_numeric($items[$delta]['target'])) {
          _fivestar_cast_vote('node', $items[$delta]['target'], $rating, $field['axis'], $node->uid, FALSE, TRUE);
          votingapi_recalculate_results('node', $items[$delta]['target']);
        }
      }
      break;
    case 'view':
      foreach ($items as $delta => $item) {
        $items[$delta]['view'] = content_format($field, $item);
      }
      return theme('field', $node, $field, $items, $teaser, $page);
  }
}

/**
 * Helper function to find the nid that should be rated when a field is changed.
 */
function fivestar_field_target($node, $field, $item) {
  $target = FALSE;

  if (!empty($field['php_target'])) {
    // Use eval rather than drupal_eval to allow access to local variables.
    $target = eval($field['php_target']);
  }
  elseif ($field['dynamic_target'] && !empty($node->$field['dynamic_target'])) {
    if (is_array($node->$field['dynamic_target']) && is_numeric($node->{$field['dynamic_target']}[0]['nid'])) {
      $target = $node->{$field['dynamic_target']}[0]['nid'];
    }
    elseif (is_numeric($node->$field['dynamic_target'])) {
      $target = $node->$field['dynamic_target'];
    }
  }
  elseif (isset($item['target'][0]['nid'])) {
    $target = $item['target'][0]['nid'];
  }

  return $target;
}

/**
 * Implementation of hook_widget_info().
 */
function fivestar_widget_info() {
  return array(
    'stars' => array(
      'label' => t('Stars'),
      'field types' => array('fivestar'),
    ),
    'radios' => array(
      'label' => t('Select list'),
      'field types' => array('fivestar'),
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
        '#type' => 'checkbox',
        '#title' => t('Allow user to clear value'),
        '#default_value' => isset($widget['allow_clear']) ? $widget['allow_clear'] : 1,
      );
      return $form;
    case 'save':
      return array('allow_clear');
  }
}

/**
 * Implementation of hook_widget().
 */
function fivestar_widget($op, &$node, $field, &$items) {
  switch ($op) {
    case 'form':
      $form = array();
      
      $form[$field['field_name']] = array('#tree' => TRUE);
      $form[$field['field_name']][0]['rating'] = array(
        '#type' => 'fivestar',
        '#title' => t($field['widget']['label']),
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

      $form[$field['field_name']][0]['target'] = array(
        '#type' => 'value',
        '#value' => $field['target'],
      );
      $form[$field['field_name']][0]['axis'] = array(
        '#type' => 'value',
        '#value' => $field['axis'],
      );
      return $form;
  }
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
  if (empty($item['rating'])) {
    $item['rating'] = 0;
  }
  if (empty($field['stars'])) {
    $field['stars'] = 5;
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
