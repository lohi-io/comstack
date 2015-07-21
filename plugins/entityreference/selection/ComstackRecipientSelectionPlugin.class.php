<?php

/**
 * Comstack recipient selection handler.
 */
class ComstackRecipientSelectionPlugin extends EntityReference_SelectionHandler_Generic_user {
  public function buildEntityFieldQuery($match = NULL, $match_operator = 'CONTAINS') {
    $query = parent::buildEntityFieldQuery($match, $match_operator);

    // The user entity doesn't have a label column.
    if (isset($match)) {
      $query->propertyCondition('name', $match, $match_operator);
    }

    // Prevent in-active/blocked users from appearing in this list.
    $query->propertyCondition('status', 1);

    // Add a tag to the query so that other modules can alter it later.
    // Use cases for this could be to restrict the list of users by role, or by
    // a friends list etc.
    $query->addTag('comstack_recipients');
    $query->addMetaData('entityreference_selection_handler', $this);

    return $query;
  }

  public function entityFieldQueryAlter(SelectQueryInterface $query) {

  }
}
