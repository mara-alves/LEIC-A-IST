package woo.exceptions;

/** Exception thrown when trying to create a client with a repeated key */
public class RepeatedClientKeyException extends Exception {

  /** Client key. */
  private String _key;

  /**
   * @param key
   */
  public RepeatedClientKeyException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
