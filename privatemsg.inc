<?php

/**
 * @file
 * Privatemag module integration
 *
 * Limitations:
 *   - No updating of messages.
 *   - No threading
 *   - Messages are marked as deleted and not actually deleted. Thats the
 *       privatemsg API.
 *   - All these limitations can be helped by http://drupal.org/node/1184984.
 */

class MigrateDestinationPrivateMsg extends MigrateDestination {

  /**
   * An array with content ids of imported messages. Not yet used.
   */
  var $importedIds = array();

  static public function getKeySchema() {
    return array(
      'mid' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Mark provided message as deleted.
   *
   * @param $id
   *  IDs to be deleted.
   */
  public function Rollback(array $id) {
    migrate_instrument_start(__METHOD__);
    privatemsg_message_change_delete($id, 1);
    migrate_instrument_stop(__METHOD__);
  }

  /**
   * Import a single message.
   *
   * @param $entity
   *  Object object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields of the object that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $entity, stdClass $row) {
    $this->prepare($entity, $row);

    // The privatemsg API does not support updating. See http://drupal.org/node/1184984
    // $message['mid'] = $entity->mid;

    $options = array();
    if (isset($entity->timestamp)) $options['timestamp'] = Migration::timestamp($entity->timestamp);
    if (isset($entity->author)) $options['author'] = $entity->author;
    if (isset($entity->format)) $options['format'] = $entity->format;

    // FYI, API is at http://api.worldempire.ch/api/privatemsg/privatemsg.module/function/privatemsg_new_thread/7-2
    $return = privatemsg_new_thread($entity->recipients, $entity->subject, $entity->body, $options);

    if ($return['message'] !== FALSE) {
      $this->complete($return, $row);
      return array($return->mid);
    }
    else {
      // TODO: parse return value and return sensible error. The PM API is odd.
    }
  }



  /**
   * Returns a list of fields available to be mapped/
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    return array(
        // 'mid' => 'Message ID', // Updating not supported. See http://drupal.org/node/1184984.
        'subject' => 'Subject',
        'body' => 'Body',
        'format' => 'Text format name for the Body',
        'recipients' => 'User IDs of recipients',
        'timestamp' => 'Timestamp',
        'author' => 'User ID of author',
    );
  }

  /**
   * Give handlers a shot at modifying the object before saving it.
   *
   * @param $entity
   *  Entity object to build. Prefilled with any fields mapped in the Migration.
   * @param $source_row
   *  Raw source data object - passed through to prepare handlers.
   */
  public function prepare(stdClass $entity, stdClass $source_row) {
    $migration = Migration::currentMigration();
    $entity->migrate = array(
      'machineName' => $migration->getMachineName(),
    );

    // Call any prepare handler for this specific Migration.
    if (method_exists($migration, 'prepare')) {
      $migration->prepare($entity, $source_row);
    }
  }

  /**
   * Give handlers a shot at modifying the object (or taking additional action)
   * after saving it.
   *
   * @param $object
   *  Entity object to build. This is the complete object after saving.
   * @param $source_row
   *  Raw source data object - passed through to complete handlers.
   */
  public function complete(stdClass $entity, stdClass $source_row) {
    $migration = Migration::currentMigration();

    // Call any complete handler for this specific Migration.
    if (method_exists($migration, 'complete')) {
      $migration->complete($entity, $source_row);
    }
  }
}
