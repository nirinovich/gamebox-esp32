import { describe, it, expect, vi } from 'vitest';
import { ConnectFourRenderer } from './ConnectFourRenderer';
import { CameraJuice } from '../../../frontend/src/renderers/CameraJuice';

describe('ConnectFourRenderer', () => {
  it('calculates correct column index on pointer down', () => {
    const dispatchSpy = vi.fn();
    const juice = new CameraJuice();
    const renderer = new ConnectFourRenderer(juice);
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    // 400 / 7 ~= 57.14 px per column
    renderer.handlePointerDown(20, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'drop', col: 0 });

    renderer.handlePointerDown(380, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'drop', col: 6 });
  });
});
