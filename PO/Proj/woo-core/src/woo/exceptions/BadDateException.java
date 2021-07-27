package woo.exceptions;

/** Exception thrown when days to advance aren't valid */
public class BadDateException extends Exception {

  /** Client key. */
  private int _days;

  /**
   * @param key
   */
  public BadDateException(int days) {
    _days = days;
  }

  /** @return days */
  public int getDays() {
    return _days;
  }

}