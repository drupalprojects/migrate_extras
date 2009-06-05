<?php
// $Id$

/**
 * @file
 * Integrates user_realtionship module with the migrate module
 * 
 */


/**
 * [Full Doxygen style documentaiton: see http://drupal.org/node/1354]
 * 
 * [Do something to something else ] 
 *
 * [Addtional info]
 *
 * @param $foo
 *   A string containing an email address.
 * @return
 *   TRUE if the address is in a valid format.
 */
function migrate_ur_migrate_init() {
  return array(
    'destination' => array(
      'user_relationship' => t('User Relationship'),
    ),
  );
}

function migrate_ur_migrate_destination_delete_user_relationship($ur) {
  user_relationships_delete_relationship($ur, $deleted_by = 1 , $op = 'remove');
}

function migrate_ur_migrate_destination_import_user_relationship($tblinfo, $row) {
  $ur = (object)array();
    
  foreach ($tblinfo->fields as $destfield => $values) {

    if ($values['srcfield'] && $row->$values['srcfield']) {
       //dpm($destfield);
       //dpm($row->$values['srcfield']);
      $ur->$destfield = $row->$values['srcfield'];
    } else {
      $ur->$destfield = $values['default_value'];
    }
  }
  dpm($ur);  
  
  timer_start('relationship_prepare hooks');
    $errors = migrate_destination_invoke_all('prepare_user_relationship', $ur, $tblinfo, $row);
  timer_stop('relationship_prepare hooks');
  
  
  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  if ($success) {
    dpm("pre_save");
    dpm($ur);
    timer_start('relationship_save');
      $ur = user_relationships_save_relationship($ur);
    timer_stop('relationship_save');
    dpm("post_save");
    dpm($ur);
    if(!$ur) {
      $errors[] = migrate_message("Relationship didn't save");
    }
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('relationship_completion hooks');
      $errors = array_merge($errors, migrate_destination_invoke_all('complete_user_relationship', $ur, $tblinfo, $row));
    timer_stop('relationship_completion hooks');
    
    $sourcekey = $tblinfo->sourcekey;
    
    //map the relationship id
    db_query("INSERT INTO {" . $tblinfo->maptable . "}
                ($sourcekey, user_relationshipid, mcsid)
                VALUES(%d, %d, %d)",
               $row->$sourcekey, $ur->rid, $tblinfo->mcsid);
    
  }
  return $errors;
}


/**
 * Implementation of hook_migrate_fields().
 */
function migrate_ur_migrate_destination_fields_user_relationship($type) {
  $fields = array();
  $fields = array(
      // 'rid' => t('UR: Relationship ID'), //this is to be set on the fly, don't need
      'requester_id' => t('UR: Requester UID'), //user requesting relationship
      'requestee_id' => t('UR: Requestee UID '), //user approving relationship 
      'rtid' => t('UR: Relationship type ID'), // id of relationship type
      'approved' => t('UR: Is approved?'), // 1 or 0
      'created_at' => t('UR: Created at'), // timestamp 
      'updated_at' => t('UR: Updated at'), //timestamp
      'flags' => t('UR: Flags'), //unknown
      $relationship_type->is_oneway
    );
    dpm($fields);
  return $fields;
}

/**
 * Implementation of hook_migrate_prepare().
 */
function migrate_ur_migrate_destination_prepare_user_relationship(&$ur, $tblinfo, $row) {
//not used.
}

function migrate_ur_migrate_destination_types() {
  $types = array();
  dpm(user_relationships_types_load());
  $relationships = user_relationships_types_load();
  foreach($relationships as $rel) {
    $types[] = $rel->name;
  }
  dpm($types);
  return $types;
}
