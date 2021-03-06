<?php

/**
 * Implements hook_theme().
 *
 * @see fett_theme()
 */
function _fett_theme_offcanvas(&$theme){
  $theme['fett_offcanvas_link'] = array(
    'variables' => array(
      'id' => '',
      'title' => '',
      'effect' => 1,
      'visibility' => '',
      'direction' => 'left',
    ),
  );
  $theme['fett_offcanvas_block'] = array(
    'variables' => array(
      'id' => '',
      'effect' => 1,
      'content' => '',
      'direction' => 'left',
    ),
  );
}

/**
 * Implements template_preprocess_page().
 *
 * @see fett_preprocess_page()
 */
function fett_offcanvas_preprocess_page(&$vars){
  foreach(array('main_nav', 'sidebar_first', 'sidebar_second') as $key){
    fett_offcanvas_element($key, $vars);
  }
}

/**
 * Offcanvas processing for main nav
 */
function fett_offcanvas_element($key, &$vars){
  $path_fett = drupal_get_path('theme', 'fett');

  // Define activation link and expose it to page.tpl.php.
  $vars['offcanvas_' . $key . '_link'] = '';

  if(($visibility = fett_get_setting('offcanvas_' . $key)) && $parents = _fett_offcanvas_element_find($vars, $key)){
    $content = drupal_array_get_nested_value($vars, _fett_offcanvas_element_find($vars, $key));
    if(!empty($content)){
      if(isset($content['#attributes']['id'])){
        $content['#attributes']['id'] .= '-oc';
      }
      if($visibility === 'always'){
        _fett_offcanvas_element_remove($vars, $parents);
      }
      else{
        _fett_offcanvas_element_wrap($vars, $parents, $visibility);
      }
      $effect = fett_get_setting('offcanvas_' . $key . '_effect');
      $direction = fett_get_setting('offcanvas_' . $key . '_direction', NULL, 'left');
      $title = fett_get_setting('offcanvas_' . $key . '_title', NULL, '<i class="fa fa-bars"></i> ' . t('View'));
      $hook = 'fett_offcanvas_' . $key;

      // Allow other modules to modify the offcanvas element.
      drupal_alter($hook, $content, $effect, $direction);

      $render = fett_offcanvas_add($content, $effect, $title, $direction, 'hide-' . $visibility, $key);
      $vars['offcanvas_' . $key . '_link'] = drupal_render($render);
    }
  }
}

/**
 * Utility class to find an element within page $vars.
 */
function _fett_offcanvas_element_find($vars, $key){
  if(isset($vars[$key])){
    return array($key);
  }
  if(isset($vars['page'][$key])){
    return array('page', $key);
  }
  return FALSE;
}

/**
 * Utility class to set visibility on an element within page $vars.
 */
function _fett_offcanvas_element_wrap(&$vars, $parents, $visibility){
  $content = &drupal_array_get_nested_value($vars, $parents);
  $content['#attributes']['class'][] = 'show-' . $visibility;
}

/**
 * Utility class to remove an element from page $vars.
 */
function _fett_offcanvas_element_remove(&$vars, $parents){
  $content = &drupal_array_get_nested_value($vars, $parents);
  $content = '';
}

/**
 * Add an element off-canvas.
 *
 * @param $content
 *   The content to place in the off-canvas element. Can be markup or a render
 *   array.
 * @param $effect
 *   The effect that will be used when showing the off-canvas element.
  *    1 => 'Slide in on top'
  *    2 => 'Reveal'
  *    3 => 'Push'
  *    4 => 'Slide along'
  *    5 => 'Reverse slide out'
  *    6 => 'Rotate pusher'
  *    7 => '3D rotate in'
  *    8 => '3D rotate out'
  *    9 => 'Scale down pusher'
  *    10 => 'Scale Up'
  *    11 => 'Scale & rotate pusher'
  *    12 => 'Open door'
  *    13 => 'Fall down'
  *    14 => 'Delayed 3D Rotate'
 * @param $title
 *   The title/markup to be used within the link that activates the off-canvas.
 * @param $direction
 *   Should be 'left' or 'right' depending on which side of the screen the
 *   off-canvas element should appear.
 * @param $visibility
 *   The Foudnation visibility class to see to the link.
 *   More info - http://foundation.zurb.com/docs/components/visibility.html
 * @param $unique_id
 *   The unique id for the off-canvas element. This will be used in the #id in
 *   both the link and off-canvas item wrapper.
 */
function fett_offcanvas_add($content, $effect = 1, $title = '<i class="fa fa-bars"></i>', $direction = 'left', $visibility = 'hide-for-medium-up', $unique_id = NULL){
  $path_fett = drupal_get_path('theme', 'fett');
  $offcanvas = &drupal_static('fett_offcanvas', array(0 => array(), 1 => array()));
  $inset_effects = array(3,6,7,8,14);
  $unique_id = $unique_id && !isset($offcanvas[$unique_id]) ? $unique_id : (count($offcanvas[0]) + count($offcanvas[1]));
  $visibility = $visibility ? $visibility : 'hide-for-medium-up';
  $title = filter_xss($title, array('strong','em','span','i'));
  $offcanvas[in_array($effect, $inset_effects)][] = array(
    '#theme' => 'fett_offcanvas_block',
    '#id' => $unique_id,
    '#effect' => $effect,
    '#content' => $content,
    '#direction' => $direction,
    '#attached' => array(
      'css' => array(
        "$path_fett/assets/scss/offcanvas/_effect$effect.scss" => array(
          'group' => CSS_DEFAULT,
          'weight' => 1, // make sure effects come after _offcanvas.scss.
        )
      ),
    ),
  );
  return array(
    '#theme' => 'fett_offcanvas_link',
    '#id' => $unique_id,
    '#title' => $title,
    '#effect' => $effect,
    '#visibility' => $visibility,
    '#direction' => $direction,
  );
}

/**
 * Get offcanvas content available on this request.
 */
function fett_offcanvas_get(){
  $offcanvas = &drupal_static('fett_offcanvas', FALSE);
  return $offcanvas;
}

/**
 * Implements template_preprocess_fett_offcanvas_block().
 */
function fett_preprocess_fett_offcanvas_block(&$vars){
  $vars['classes_array'][] = 'oc-block';
  $vars['classes_array'][] = 'oc-effect-' . $vars['effect'];
  if($vars['id'] === 'sidebar_second'){
    $vars['direction'] = 'right';
  }
  $vars['classes_array'][] = 'oc-direction-' . $vars['direction'];
  $vars['attributes_array']['id'] = 'oc-block-' . drupal_html_class($vars['id']);
}

/**
 * Implements template_process_fett_offcanvas_block().
 */
function fett_process_fett_offcanvas_block(&$vars){
  $vars['attributes'] = empty($vars['attributes_array']) ? '' : drupal_attributes($vars['attributes_array']);
  $vars['classes'] = empty($vars['classes_array']) ? '' : implode(' ', $vars['classes_array']);
  // Allow both render arrays and strings to be passed for rendering.
  if(is_string($vars['content'])){
    $vars['content'] = array(
      '#markup' => $vars['content'],
    );
  }
}

/**
 * Return themed output for fett_offcanvas_block.
 */
function fett_fett_offcanvas_block($vars){
  $output = '<div' . $vars['attributes'] . ' class="' . $vars['classes'] . '">';
  $output .= drupal_render($vars['content']);
  $output .= '<a class="oc-close" href="#"></a></div>';
  return $output;
}

/**
 * Implements template_preprocess_fett_offcanvas_link().
 */
function fett_preprocess_fett_offcanvas_link(&$vars) {
  $vars['attributes_array']['id'] = 'oc-link-' . drupal_html_class($vars['id']);
  $vars['attributes_array']['data-oc-effect'] = $vars['effect'];
  $vars['attributes_array']['data-oc-id'] = drupal_html_class($vars['id']);
  if($vars['id'] === 'sidebar_second'){
    $vars['direction'] = 'right';
  }
  $vars['attributes_array']['data-oc-direction'] = $vars['direction'];
  $vars['classes_array'][] = 'oc-link';
  if($vars['visibility'] !== 'always'){
    $vars['classes_array'][] = $vars['visibility'];
  }
}

/**
 * Implements template_process_fett_offcanvas_link().
 */
function fett_process_fett_offcanvas_link(&$vars) {
  $vars['attributes'] = empty($vars['attributes_array']) ? '' : drupal_attributes($vars['attributes_array']);
  $vars['classes'] = empty($vars['classes_array']) ? '' : implode(' ', $vars['classes_array']);
}

/**
 * Return themed output for fett_offcanvas_link.
 */
function fett_fett_offcanvas_link($vars) {
  return '<span ' . $vars['attributes'] . ' class="' . $vars['classes'] . '"><a href="#">' . $vars['title'] . '</a></span>';
}
