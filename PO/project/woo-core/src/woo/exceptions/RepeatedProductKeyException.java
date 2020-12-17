package woo.exceptions;

/** Exception thrown when trying to create a product with a repeated key */
public class RepeatedProductKeyException extends Exception {

  /** Product key. */
  private String _key;

  /**
   * @param key
   */
  public RepeatedProductKeyException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
