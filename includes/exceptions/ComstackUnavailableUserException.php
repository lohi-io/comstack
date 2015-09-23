<?php

/**
 * @file
 * Contains \ComstackUnavailableUserException.
 */

class ComstackUnavailableUserException extends ComstackException {
  protected string $message = "The user you're attempting to interact with isn't available to you.";
}
