<?php
// $Id$

/**
 * @file
 * Handler for handling FileFields in destination nodes.
 */

class MigrateFileFieldHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('filefield'));
  }

  /**
   * Prepare file data for saving as a Field API file field. The job of this
   * handler is to make sure the file itself ends up in the right place, the
   * files table row is set up, and the files array returned for each file.
   *
   * @return array
   *  Field API array suitable for inserting in the destination object.
   */
  public function prepare(Migration $migration, stdClass $entity, array $instance, array $values) {
    $return = array();
    $arguments = $values['arguments'];
    unset($values['arguments']);
    // One can override a file_function via CLI or drushrc.php
    if ($migration->getOption('file_function')) {
      $file_function = $migration->getOption('file_function');
    }
    else {
      $file_function = $arguments['file_function'];
    }
    foreach ($values as $delta => $file_path) {
      if ($arguments['source_path']) {
        $full_path = rtrim($arguments['source_path'], DIRECTORY_SEPARATOR) .
          DIRECTORY_SEPARATOR . ltrim($file_path, DIRECTORY_SEPARATOR);
      }
      else {
        $full_path = $file_path;
      }

      $destination_dir = file_directory_path() . DIRECTORY_SEPARATOR . $instance['widget']['file_path'];
      $destination_file = rtrim($destination_dir, DIRECTORY_SEPARATOR) .
        DIRECTORY_SEPARATOR . basename($full_path);
      migrate_instrument_start('MigrateFileFieldHandler file_function');
      switch ($file_function) {
        // These physically transfer the file to the destination, applying
        // the file_replace setting if the file already exists
        case 'file_copy':
        case 'file_move':
          // Check that source exists. If not, mark the entity as 'needs_update' and bail.
          // Sometimes the source file arrives later, when rsync is slower than DB.
          if (!file_exists($full_path)) {
            $migration->saveMessage(t('Source file does not exist: !path',
              array('!path' => $full_path)), MigrationBase::MESSAGE_INFORMATIONAL);
      // TODO     $migration->needsUpdate = TRUE;
            continue;
          }
          // TODO: FILE_EXISTS_REPLACE is hardcoded
          // TODO: Should retrieve and pass validators in 2nd arg
          $file = field_file_save_file($full_path, array(), $destination_dir);
          if ($file_function == 'file_move') {
            // TODO: Optimize by using rename()
            unlink($full_path);
          }
          break;
        case 'file_fast':
          static $fid;
          // Keep re-using an existing file.
          if (!isset($fid)) {
            $full_path = 'misc/druplicon.png';
            $file = field_file_save_file($full_path, array(), $destination_dir);
  //          $file = file_copy($source, $destination_dir, FILE_EXISTS_RENAME);
            $fid = $file['fid'];
          }
          else {
            $file = array('fid' => $fid);
          }
          break;
        case 'file_link':
          // The file is copied by some outside process (e.g., rsync), and we
          // just need to make sure it's present and has a files table row.
          // Not present - skip
          if (!file_exists($full_path)) {
            drush_print("doesn't exist: $full_path");
            $migration->saveMessage(t('File does not exist in Drupal files directory: !path',
              array('!path' => $full_path)), MigrationBase::MESSAGE_INFORMATIONAL);
      // TODO     $migration->needsUpdate = TRUE;
            continue;
          }

          // Files table entry exists? Use that...
          $file = db_select('files', 'f')
                  ->fields('f')
                  ->condition('filepath', $full_path)
                  ->execute()
                  ->fetchAssoc();
          if (!$file) {
            $file = new stdClass;
            $file->uri = $full_path;
            $file->uid = isset($entity->uid) ? $entity->uid : 0;
            $file->filename = basename($full_path);
            $file->filepath = $full_path;
            $file->filemime = file_get_mimetype($full_path);
            $file->timestamp = $_SERVER['REQUEST_TIME'];
            $file->filesize = filesize($full_path);
            $file->status = FILE_STATUS_PERMANENT;
            drupal_write_record('files', $file);
            $file = (array)$file;
          }
          break;
        default:
          $migration->saveMessage(t('Unrecognized file_function: !func', array('!func' => $file_function)));
          break;
      }
      migrate_instrument_stop('MigrateFileFieldHandler file_function');

      if ($file) {
        // Build up a return object.
        $file['list'] = NULL;
  /*      $object_field['fid'] = $file->fid;
        $object_field['alt'] = isset($arguments['source_alt_name']) ? $values[$arguments['source_alt_name']] : NULL;
        $object_field['title'] = isset($arguments['source_title_name']) ? $values[$arguments['source_title_name']] : NULL;
        $object_field['description'] = isset($arguments['source_description_name']) ? $values[$arguments['source_description_name']] : NULL;
        $object_field['display'] = isset($arguments['source_display_name']) ? $values[$arguments['source_display_name']] : NULL;*/
        $return[] = $file;
      }
      else {
        $migration->saveMessage(t('Unable to create file record for !path', array('!path' => $full_path)), Migration::MESSAGE_ERROR);
      }
    }
    return $return;
  }

  /*
   * Arguments for a file_field migration.
   *
   * @param source_path
   *   Path to source file.
   * @param file_function
   *   file_fast, file_move, or file_copy.
   * @param file_replace
   *   Value of $replace in that file function. Does not apply to file_fast(). Defaults to FILE_EXISTS_RENAME.
   * @param language
   *   Language of the text (defaults to destination language)
   * @param source_alt_name
   * @param source_title_name
   * @param source_description_name
   * @param source_display_name
   *
   */
 static function arguments($source_path = NULL, $file_function = 'file_copy',
      $file_replace = FILE_EXISTS_RENAME, $source_alt_name = NULL,
      $source_title_name = NULL, $source_description_name = NULL,
      $source_display_name = NULL) {
    return get_defined_vars();
  }
}
