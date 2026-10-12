export interface IGameRenderer {
  init(canvas: HTMLCanvasElement, ctx: CanvasRenderingContext2D): void;
  resize(width: number, height: number): void;
  render(state: any, ctx: CanvasRenderingContext2D): void;
  handlePointerDown(x: number, y: number): void;
  handlePointerMove(x: number, y: number): void;
  handlePointerUp(x: number, y: number): void;
  destroy(): void;
}
