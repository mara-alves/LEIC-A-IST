package woo.exceptions;

/** Exception thrown when service level doesn't exist */
public class NoSuchServiceLevelException extends Exception {

  /** Service level. */
  private String _serviceLevel;

  /**
   * @param serviceType
   */
  public NoSuchServiceLevelException(String serviceLevel) {
    _serviceLevel = serviceLevel;
  }

  /** @return service level */
  public String getServiceLevel() {
    return _serviceLevel;
  }

}