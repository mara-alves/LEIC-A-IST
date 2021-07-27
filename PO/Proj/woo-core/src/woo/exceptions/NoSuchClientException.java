package woo.exceptions;

/** Exception thrown when client requested by key doesn't exist */
public class NoSuchClientException extends Exception {

  /** Client key. */
  private String _key;

  /**
   * @param key
   */
  public NoSuchClientException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
