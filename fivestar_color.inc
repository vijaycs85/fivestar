<?php
// $Id$

/**
 * Form callback. Returns the configuration form.
 */
function fivestar_color_form() {
  $form = array(
    '#submit' => array('fivestar_color_form_submit'),
    '#tree' => FALSE,
    '#theme' => 'fivestar_color_form',
    '#type' => 'fieldset',
    '#title' => t('Color scheme'),
    '#weight' => -1,
    '#attributes' => array('id' => 'fivestar_color_scheme_form'),
  );

  // Create the list of available schemes.
  $fivestar_schemes = array(
    '#ffeb38,#fff385,#ffd60a,#ffe561,#8f8f8f,#e6e6e6' => t('Yellow'),
    '#ad0002,#ff5f61,#ff1a1f,#f3b3b4,#8f8f8f,#e6e6e6' => t('Red'),
    '#001efa,#bbc3fb,#1022c1,#2e41ee,#8f8f8f,#e6e6e6' => t('Blue'),
    '#015700,#7cf47c,#12c610,#c8f9c7,#8f8f8f,#e6e6e6' => t('Green'),
  );

  // Add schemes provided by themes if color module is available.
  if (module_exists('color')) {
    $theme = variable_get('theme_default', 'garland');
    $theme_info = color_get_info($theme);
    $themes = list_themes();
    $theme_name = $themes[$theme]->name;
  }

  $theme_schemes = array();
  if (!empty($theme_info['schemes'])) {
    foreach ($theme_info['schemes'] as $key => $name) {
      $colors = explode(',', $key);
      $theme_scheme = array(
        'on1' => $colors[0],
        'on2' => _fivestar_color_blend($colors[0], '#FFFFFF', .8),
        'hover1' => $colors[2],
        'hover2' => _fivestar_color_blend($colors[2], '#FFFFFF', .8),
        'off1' => $colors[4],
        'off2' => _fivestar_color_blend($colors[4], '#FFFFFF', .8),
      );
      $theme_schemes[implode(',', $theme_scheme)] = $name;
    }
  }

  if (!empty($theme_schemes)) {
    $options = array(
      t('Fivestar schemes') => $fivestar_schemes,
      t('@theme_name schemes', array('@theme_name' => drupal_ucfirst($theme_name))) => $theme_schemes,
    );
  }
  else {
    $options = $fivestar_schemes;
  }
  $options[''] = t('Custom');

  // See if we're using a predefined scheme.
  $yellow_palette = explode(',', array_search(t('Yellow'), $fivestar_schemes));
  $default_palette['on1'] = $yellow_palette[0];
  $default_palette['on2'] = $yellow_palette[1];
  $default_palette['hover1'] = $yellow_palette[2];
  $default_palette['hover2'] = $yellow_palette[3];
  $default_palette['off1'] = $yellow_palette[4];
  $default_palette['off2'] = $yellow_palette[5];
  $default_palette['matte'] = '#ffffff';

  $palette = variable_get('fivestar_colors', $default_palette);
  $scheme_default = implode(',', array_slice($palette, 0, count($palette) - 1));

  $form['fivestar_color_type'] = array(
    '#type' => 'select',
    '#title' => t('Color display'),
    '#options' => array('solid' => t('Solid color'), 'gradient' => t('Gradient')),
    '#default_value' => variable_get('fivestar_color_type', 'solid'),
  );

  // Add scheme selector.
  $form['scheme'] = array(
    '#type' => 'select',
    '#title' => t('Color set'),
    '#options' => $options,
    '#default_value' => isset($options[$scheme_default]) ? $scheme_default : '',
  );

  // Add palette fields.
  $names = array(
    'on1' => t('On colors'),
    'on2' => NULL,
    'hover1' => t('Hover colors'),
    'hover2' => NULL,
    'off1' => t('Off colors'),
    'off2' => NULL,
    'matte' => t('Matte color'),
  );

  $form['fivestar_colors']['#tree'] = true;
  $form['fivestar_colors']['#theme'] = 'fivestar_color_palette';
  foreach ($names as $key => $name) {
    $form['fivestar_colors'][$key] = array(
      '#type' => 'textfield',
      '#title' => $name,
      '#default_value' => $palette[$key],
      '#size' => 8,
    );
  }

  return $form;
}

/**
 * Theme color form.
 */
function theme_fivestar_color_form($form) {
  // Add Farbtastic color picker.
  drupal_add_css('misc/farbtastic/farbtastic.css', 'module', 'all', FALSE);
  drupal_add_js(drupal_get_path('module', 'fivestar') .'/js/fivestar-color.js');
  drupal_add_js('misc/farbtastic/farbtastic.js');

  // Add custom CSS/JS.
  $default_colors = array();
  foreach (element_children($form['fivestar_colors']) as $key) {
    $default_colors[$key] = $form['fivestar_colors'][$key]['#value'];
  }

  drupal_add_js(array('fivestar' => array('reference' => $default_colors, 'transparent' => t('none'), 'colorPreview' => url('fivestar/preview/color'))), 'setting');

  // Wrapper.
  $output .= '<div class="color-form clear-block">';

  // Color schemes.
  $output .= drupal_render($form['fivestar_color_type']);
  $output .= drupal_render($form['scheme']);

  // Palette.
  $output .= '<div id="fivestar-palette" class="clear-block">';
  foreach (element_children($form['fivestar_colors']) as $key => $name) {
    // Render pairs on a single line inside a new form element.
    if (strpos($name, '1')) {
      $name2 = str_replace('1', '2', $name);
      $title = $form['fivestar_colors'][$name]['#title'];
      unset($form['fivestar_colors'][$name]['#title']);
      $element = array(
        '#type' => 'element',
        '#title' => $title,
        '#children' => drupal_render($form['fivestar_colors'][$name]) . drupal_render($form['fivestar_colors'][$name2]),
      );
      $output .= theme('form_element', $element, $element['#children']);
    }
    $output .= drupal_render($form['fivestar_colors'][$name]);
  }
  $output .= '</div>';

  // Render the form.
  $output .= drupal_render($form);
  // Close wrapper.
  $output .= '</div>';

  return $output;
}

/**
 * Validate handler for color form.
 */
function fivstar_color_form_validate($form, &$form_state) {
  foreach ($form_state['values']['fivestar_colors'] as $key => $value) {
    $form_state['values']['fivestar_colors'][$key] = trim(drupal_strtolower($value));
    if (!preg_match('/^#([0-9a-f]{3}|[0-9a-f]{6})$/', $form_state['values']['fivestar_colors'][$key]) && !($key == 'matte' && ($value == 'none' || $value == ''))) {
      form_set_error('fivestar_colors]['. $key, t('The entered value %color is not a valid hex color.', array('%color' => $value)));
    }
  }
}

/**
 * Submit handler for color change form.
 */
function fivstar_color_form_submit($form, &$form_state) {
  $form = fivestar_settings();

  if ($form_state['values']['matte'] == t('none')) {
    $form_state['values']['matte'] = '';
  }

  // Check if we're using a color-enabled set of stars.
  if (!array_key_exists($form_state['values']['fivestar_widget'], $form['widget']['fivestar_color_widget']['#options'])) {
    return;
  }

  $widget = str_replace('.css', '', basename($form_state['values']['fivestar_widget']));
  $widget_css = $form_state['values']['fivestar_widget'];
  $widget_rtl_css = str_replace('.css', '-rtl.css', $widget_css);
  $upload_directory = file_directory_path() .'/fivestar';
  $widget_directory = $upload_directory .'/'. $widget;
  $current_widget = variable_get('fivestar_widget', 'default');

  // Delete the previous set of stars if any from the files directory.
  if (strpos($current_widget, $upload_directory) === 0) {
    $current_widget_directory = dirname($current_widget);
    file_scan_directory($current_widget_directory, '.*', array('.', '..', 'CVS'), 'unlink');
    rmdir($current_widget_directory);
  }

  // Check the destination directory.
  file_check_directory($upload_directory, FILE_CREATE_DIRECTORY);
  file_check_directory($widget_directory, FILE_CREATE_DIRECTORY);

  // Create the new stars.
  $star = _fivestar_color_render(str_replace($widget .'.css', 'star-template.png', $form_state['values']['fivestar_widget']), $form_state['values']['fivestar_colors'], $form_state['values']['fivestar_color_type']);
  $cancel = _fivestar_color_render(str_replace($widget .'.css', 'cancel-template.png', $form_state['values']['fivestar_widget']), $form_state['values']['fivestar_colors'], $form_state['values']['fivestar_color_type']);
  imagepng($star, $widget_directory .'/star.png');
  imagepng($cancel, $widget_directory .'/cancel.png');

  // Copy over the stylesheet.
  file_copy($widget_css, $widget_directory .'/'. $widget .'.css', FILE_EXISTS_REPLACE);

  // Copy over RTL stylesheet.
  if (file_exists($widget_rtl_css)) {
    file_copy($widget_rtl_css, $widget_directory .'/'. $widget .'-rtl.css', FILE_EXISTS_REPLACE);
  }

  $form_state['values']['fivestar_widget'] = $widget_css;

  drupal_set_message(t('Custom %name stars generated. You may need to clear your browser cache before the new stars are visible.', array('%name' => t($widget))));
}

/**
 * Render images that match a given palette.
 * @param $source
 *   The original image source (star-template.png or cancel-template.png).
 * @param $palette
 *   The colors to be used in the generation of this image.
 * @param $type
 *   The type of color to be rendered: solid or gradient.
 */
function _fivestar_color_render($source, $palette, $type) {

  // Prepare template image.
  $template = imagecreatefrompng($source);
  imagealphablending($template, true);

  $width = imagesx($template) / 2;
  $height = imagesy($template);

  $mask = imagecreatetruecolor($width, $height);
  $transparent = imagecolorallocatealpha($mask, 0, 0, 0, 127);
  imagefill($mask, 0, 0, $transparent);
  imagecopy($mask, $template, 0, 0, 0, 0, $width, $height);

  $overlay = imagecreatetruecolor($width, $height);
  $transparent = imagecolorallocatealpha($overlay, 0, 0, 0, 127);
  imagefill($overlay, 0, 0, $transparent);
  imagecopy($overlay, $template, 0, 0, $width, 0, $width, $height);

  imagedestroy($template);

  // Apply color mask.
  $slices = (basename($source) == 'star-template.png') ? 3 : 2;
  $slice_height = floor($height / $slices);

  foreach ($slices == 2 ? array('off', 'hover') : array('off', 'on', 'hover') as $slice => $key) {
    $slice_y = $slice_height * $slice;
    if ($type == 'gradient') {
      _fivestar_color_mask_linear_gradient($mask, $palette[$key .'1'], $palette[$key .'2'], $palette['matte'], 0, $slice_y, $width, $slice_height);
    }
    else {
      _fivestar_color_mask($mask, $palette[$key .'1'], $palette['matte'], 0, $slice_y, $width, $slice_height);
    }
  }

  // Apply the overlay on top of the mask.
  imagealphablending($mask, true);
  imagecopy($mask, $overlay, 0, 0, 0, 0, $width, $height);
  imagedestroy($overlay);

  // Set the background color.
  if ($palette['matte'] == 'transparent') {
    // A simple case, just make this save as a 24-bit PNG.
    imagesavealpha($mask, true);
    $return = $mask;
  }
  else {
    // If there is a matte, create a new 8-bit image, fill with the matte,
    // apply the star over the top, then set the matte to transparent.
    $return = imagecreate($width, $height);
    $matte_rgb = _fivestar_color_unpack($palette['matte']);
    $transparent = imagecolorallocate($return, $matte_rgb[0], $matte_rgb[1], $matte_rgb[2]);
    imagecolortransparent($return, $transparent);
    imagealphablending($return, true);
    imagecopy($return, $mask, 0, 0, 0, 0, $width, $height);
  }

  return $return;
}

/**
 * Apply a color to a portion of a black and white mask image.
 * 
 * @param $mask
 *   A GD image reference containing the mask image.
 * @param $color
 *   A hex color value (i.e. ffdd00) to apply to the mask.
 */
function _fivestar_color_mask(&$mask, $color, $matte_color, $x_offset, $y_offset, $width, $height) {
  $rgb = _fivestar_color_unpack($color);
  for ($x = $x_offset; $x < $x_offset + $width; $x++) {
    for ($y = $y_offset; $y < $y_offset + $height; $y++) {
      $current_pixel = imagecolorsforindex($mask, imagecolorat($mask, $x, $y));
      $new_color = imagecolorallocatealpha($mask, $rgb[0], $rgb[1], $rgb[2], $current_pixel['alpha']);
      // Matte coloring:
      if ($matte_color != 'transparent' && $current_pixel['alpha'] != 127) {
        $matte_rgb = _fivestar_color_unpack($matte_color);
        $matte = imagecolorallocate($mask, $matte_rgb[0], $matte_rgb[1], $matte_rgb[2]);
        imagealphablending($mask, false);
        imagesetpixel($mask, $x, $y, $matte);
        imagealphablending($mask, true);
        imagesetpixel($mask, $x, $y, $new_color);
      }
      // Transparent matte:
      else {
        imagealphablending($mask, false);
        imagesetpixel($mask, $x, $y, $new_color);
        imagealphablending($mask, true);
      }
    }
  }
}

/**
 * Apply a gradient to a portion of a black and white mask image.
 * 
 * The two colors passed in will form a linear gradient from top to bottom.
 * 
 * @param $mask
 *   A GD image reference containing the mask image.
 * @param $color1
 *   The color used at the top of the linear gradient.
 * @param $color2
 *   The color used at the bottom of the linear gradient.
 */
function _fivestar_color_mask_linear_gradient(&$mask, $color1, $color2, $matte_color, $x_offset, $y_offset, $width, $height) {
  for ($y = $y_offset; $y < $y_offset + $height; $y++) {
    $color = _fivestar_color_blend($color1, $color2, ($y - $y_offset) / $height);
    _fivestar_color_mask($mask, $color, $matte_color, 0, $y, $width, 1);
  }
}

/**
 * Blend two hex colors and return the resulting hex color.
 */
function _fivestar_color_blend($hex1, $hex2, $alpha) {
  $in1 = _fivestar_color_unpack($hex1);
  $in2 = _fivestar_color_unpack($hex2);
  for ($i = 0; $i < 3; ++$i) {
    $out[] = $in1[$i] + ($in2[$i] - $in1[$i]) * $alpha;
  }
  return _fivestar_color_pack($out);
}

/**
 * Convert a hex color into an RGB triplet.
 */
function _fivestar_color_unpack($hex, $normalize = false) {
  if (strlen($hex) == 4) {
    $hex = $hex[1] . $hex[1] . $hex[2] . $hex[2] . $hex[3] . $hex[3];
  }
  $c = hexdec($hex);
  for ($i = 16; $i >= 0; $i -= 8) {
    $out[] = (($c >> $i) & 0xFF) / ($normalize ? 255 : 1);
  }
  return $out;
}

/**
 * Convert an RGB triplet to a hex color.
 */
function _fivestar_color_pack($rgb, $normalize = false) {
  foreach ($rgb as $k => $v) {
    $out |= (($v * ($normalize ? 255 : 1)) << (16 - $k * 8));
  }
  return '#'. str_pad(dechex($out), 6, 0, STR_PAD_LEFT);
}
