package org.atriasoft.gale.context;

import org.atriasoft.etk.math.Vector2f;

/**
 * Information about a physical monitor (workspace) attached to the system.
 *
 * @param index   0-based index of the monitor (0 = primary).
 * @param id      Backend-specific identifier (e.g. AWT GraphicsDevice IDString), useful for
 *                persisting "this window was on monitor X" across sessions.
 * @param origin  Top-left corner of the monitor in the virtual desktop coordinate space.
 * @param size    Pixel size of the monitor.
 * @param primary True if this is the system primary monitor.
 */
public record MonitorInfo(int index, String id, Vector2f origin, Vector2f size, boolean primary) {}
