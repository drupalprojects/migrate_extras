<?php
/**
 * @file
 * migrate_extras_media.file_default_displays.inc
 */

/**
 * Implements hook_file_default_displays().
 */
function migrate_extras_media_file_default_displays() {
  $export = array();

  $file_display = new stdClass;
  $file_display->api_version = 1;
  $file_display->name = 'video__default__media_youtube_image';
  $file_display->weight = 0;
  $file_display->status = TRUE;
  $file_display->settings = array(
    'image_style' => '',
  );
  $export['video__default__media_youtube_image'] = $file_display;

  $file_display = new stdClass;
  $file_display->api_version = 1;
  $file_display->name = 'video__default__media_youtube_video';
  $file_display->weight = 0;
  $file_display->status = TRUE;
  $file_display->settings = array(
    'width' => '560',
    'height' => '340',
    'autoplay' => 0,
  );
  $export['video__default__media_youtube_video'] = $file_display;

  return $export;
}
