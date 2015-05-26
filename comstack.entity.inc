<?php
/**
 * @file
 * comstack.entity.inc
 */

/**
 * Implements hook_entity_info().
 */
function comstack_entity_info() {
  $entities = array();

  $entities['comstack_message_type'] = array(
    'label' => t('Message Type'),
    'controller class' => 'EntityAPIControllerExportable',
    'base table' => 'comstack_message_type',
    'fieldable' => TRUE,
    'bundle of' => 'comstack_message',
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'id',
      'name' => 'type',
      'label' => 'label',
    ),
    'module' => 'comstack',
    'admin ui' => array(
      'path' => 'admin/structure/comstack/types',
      'controller class' => 'ComstackMessageTypeUIController',
    ),
    'access callback' => 'comstack_message_type_access_callback',
  );

  $entities['comstack_message'] = array(
    'label' => t('Message'),
    'plural label' => t('Messages'),
    'base table' => 'comstack_message',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'thread_id',
      'bundle' => 'type',
      'label' => 'title',
    ),
    'bundle keys' => array(
      'bundle' => 'type',
    ),
    'bundles' => array(
      // Supplied later.
    ),
    'load hook' => 'comstack_message_load',
    'view modes' => array(
      'full' => array(
        'label' => t('Default'),
        'custom settings' => FALSE,
      ),
    ),
    'module' => 'harmony_core',
    'access callback' => 'comstack_message_access_callback',
  );

  // Add bundle info but bypass entity_load() as we cannot use it here.
  $types = db_select('comstack_message_type', 'cmt')
    ->fields('cmt')
    ->execute()
    ->fetchAllAssoc('type');

  foreach ($types as $type => $info) {
    $entities['comstack_message']['bundles'][$type] = array(
      'label' => $info->label,
      'admin' => array(
        'path' => 'admin/structure/comstack/types/%comstack_message_type',
        'real path' => 'admin/structure/comstack/types/' . $type,
        'bundle argument' => 4,
        'access arguments' => array('administer comstack settings'),
      ),
    );
  }

  return $entities;
}

/**
 * Implements hook_entity_property_info().
 */
function comstack_entity_property_info() {
  $info = array();

  // Message type entity.
  $message_type_properties = &$info['comstack_message_type']['properties'];
  $message_type_properties = array();

  // Conversation entity.
  $conversation_properties = &$info['comstack_conversation']['properties'];

  $conversation_properties['conversation_id'] = array(
    'label' => t('Conversation ID'),
    'type' => 'integer',
    'description' => t('The unique conversation ID.'),
  );
  $conversation_properties['type'] = array(
    'label' => t('Conversation/Message type'),
    'type' => 'token',
    'description' => t('The type of the conversation or message.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack_message_types',
    'options list' => 'comstack_get_message_types_flat',
    'required' => TRUE,
    'schema field' => 'type',
  );
  $conversation_properties['entity_type'] = array(
    'label' => t('Sender Entity Type'),
    'type' => 'token',
    'description' => t('The type of entity which started the conversation.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'required' => TRUE,
    'schema field' => 'entity_type',
  );
  $conversation_properties['entity_id'] = array(
    'label' => t('Sender Entity ID'),
    'type' => 'integer',
    'description' => t('The ID of the entity which started the conversation.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'required' => TRUE,
    'schema field' => 'entity_id',
  );
  $conversation_properties['entity']  = array(
    'label' => t('Sender Entity'),
    'type' => 'entity',
    'computed' => TRUE,
    'getter callback' => 'comstack_conversation_get_entity',
  );
  $conversation_properties['started'] = array(
    'label' => t('Date started'),
    'type' => 'date',
    'description' => t('The date the conversation was created.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'started',
  );
  $conversation_properties['updated'] = array(
    'label' => t('Date last updated'),
    'type' => 'date',
    'description' => t('The date the conversation was most recently updated.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'updated',
  );
  $conversation_properties['title'] = array(
    'label' => t('Title'),
    'description' => t('The title of the conversation.'),
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'title',
  );
  $conversation_properties['messages_count'] = array(
    'label' => t('Messages count'),
    'description' => t('Number of messages in the conversation.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'messages_count',
  );
  $conversation_properties['pinned'] = array(
    'label' => t('Pinned'),
    'description' => t('Whether the conversation should be displayed at the top of lists in which it appears.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'pinned',
    'type' => 'boolean',
  );
  $conversation_properties['deleted'] = array(
    'label' => t('Deleted'),
    'description' => t('Whether the conversation has been deleted.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'deleted',
    'type' => 'boolean',
  );

  // Message entity.
  $message_properties = &$info['comstack_message']['properties'];

  $message_properties['message_id'] = array(
    'label' => t('Message ID'),
    'type' => 'integer',
    'description' => t('The unique message ID.'),
  );
  $message_properties['type'] = array(
    'label' => t('Conversation/Message type'),
    'type' => 'token',
    'description' => t('The type of the conversation or message.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack_message_types',
    'options list' => 'comstack_get_message_types_flat',
    'required' => TRUE,
    'schema field' => 'type',
  );
  $message_properties['sender_entity_type'] = array(
    'label' => t('Sender Entity Type'),
    'type' => 'token',
    'description' => t('The type of entity which sent the message.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'required' => TRUE,
    'schema field' => 'sender_entity_type',
  );
  $message_properties['sender_entity_id'] = array(
    'label' => t('Sender Entity ID'),
    'type' => 'integer',
    'description' => t('The ID of the entity which sent the message.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'required' => TRUE,
    'schema field' => 'sender_entity_id',
  );
  $message_properties['sender_entity']  = array(
    'label' => t('Sender Entity'),
    'type' => 'entity',
    'computed' => TRUE,
    'getter callback' => 'comstack_conversation_get_sender',
  );
  $message_properties['entity_type'] = array(
    'label' => t('Destination Entity Type'),
    'type' => 'token',
    'description' => t('The type of destination entity.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'required' => TRUE,
    'schema field' => 'entity_type',
  );
  $message_properties['entity_id'] = array(
    'label' => t('Destination Entity ID'),
    'type' => 'integer',
    'description' => t('The ID of the destination entity.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'required' => TRUE,
    'schema field' => 'entity_id',
  );
  $message_properties['entity']  = array(
    'label' => t('Sender Entity'),
    'type' => 'entity',
    'computed' => TRUE,
    'getter callback' => 'comstack_conversation_get_entity',
  );
  $message_properties['sent'] = array(
    'label' => t('Date sent'),
    'type' => 'date',
    'description' => t('The date the message was sent.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'sent',
  );
  $message_properties['subject'] = array(
    'label' => t('Subject'),
    'description' => t('The message subject.'),
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'subject',
  );
  $message_properties['text'] = array(
    'label' => t('Text'),
    'description' => t('Message text.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'subject',
  );
  // No entries for text_replacements or actions properties.
  $message_properties['all_have_read'] = array(
    'label' => t('All have read'),
    'description' => t('Whether this message has been read by all recipients.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'all_have_read',
    'type' => 'boolean',
  );
  $message_properties['read_by_updated'] = array(
    'label' => t('Date sent'),
    'type' => 'date',
    'description' => t('Timestamp when the read by meta info was last updated.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'read_by_updated',
  );
  $message_properties['pinned'] = array(
    'label' => t('Pinned'),
    'description' => t('Whether the message should be displayed at the top of lists in which it appears.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'pinned',
    'type' => 'boolean',
  );
  $message_properties['deleted'] = array(
    'label' => t('Deleted'),
    'description' => t('Whether the message has been deleted.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comstack content',
    'schema field' => 'deleted',
    'type' => 'boolean',
  );

  return $info;
}

/**
 * Custom entity getter callback used from metadata info.
 */
function comstack_conversation_get_entity($data, array $options, $name, $type, $info) {
  return entity_load_single($data->entity_type, $data->entity_id);
}

/**
 * Custom entity getter callback used from metadata info.
 */
function comstack_conversation_get_sender($data, array $options, $name, $type, $info) {
  return entity_load_single($data->sender_entity_type, $data->sender_entity_id);
}

/**
 * Message type Entity functions.
 */

/**
 * Message type entity access callback.
 */
function comstack_message_type_access_callback($op, $entity = NULL, $account = NULL) {
  return user_access('administer comstack_message_types', $account);
}

/**
 * Return a list of message types in an array, whole objects.
 */
function comstack_get_message_types($type_name = NULL) {
  $types = entity_load_multiple_by_name('comstack_message_type', isset($type_name) ? array($type_name) : FALSE);

  return isset($type_name) ? reset($types) : $types;
}

/* Entity CRUD functions. */

/**
 * Menu argument loader; Load a message type by string.
 *
 * @param $type
 *   The machine-readable name of a message type to load.
 * @return
 *   A message type array or FALSE if $type does not exist.
 */
function comstack_message_type_load($type) {
  return comstack_get_message_types($type);
}

/**
 * Entity load function.
 */
function comstack_message_load($message_id, $reset = FALSE) {
  $messages = comstack_message_load_multiple(array($message_id), array(), $reset);
  return reset($messages);
}

/**
 * Load multiple messages based on certain conditions.
 */
function comstack_message_load_multiple($message_ids = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('comstack_message', $message_ids, $conditions, $reset);
}
