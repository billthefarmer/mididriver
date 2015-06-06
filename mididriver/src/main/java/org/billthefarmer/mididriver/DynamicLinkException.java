package org.billthefarmer.mididriver;

/**
 * Created by jhindin on 03/06/15.
 */
public class DynamicLinkException extends LinkageError {
    public DynamicLinkException() {
    }

    public DynamicLinkException(String detailMessage) {
        super(detailMessage);
    }
}
