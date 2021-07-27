package woo.exceptions;

/** Exception thrown when transaction requested by key doesn't exist */
public class NoSuchTransactionException extends Exception {

  /** Transaction key. */
  private int _key;

  /**
   * @param key
   */
  public NoSuchTransactionException(int key) {
    _key = key;
  }

  /** @return key */
  public int getKey() {
    return _key;
  }

}
