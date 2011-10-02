<?php
/**
 * @file
 * migrate_extras_media.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function migrate_extras_media_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "file_entity" && $api == "file_default_displays") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function migrate_extras_media_node_info() {
  $items = array(
    'migrate_example_media' => array(
      'name' => t('migrate_example_media'),
      'base' => 'node_content',
      'description' => t('Content type for demonstrating and testing migration of media destinations and fields.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
