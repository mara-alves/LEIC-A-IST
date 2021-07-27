package woo.exceptions;

/** Exception thrown when product requested by key doesn't exist */
public class NoSuchProductException extends Exception {

  /** Client key. */
  private String _key;

  /**
   * @param key
   */
  public NoSuchProductException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
