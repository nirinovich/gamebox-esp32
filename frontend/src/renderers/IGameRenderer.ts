export type ActionDispatcher = (action: any) => void;

export interface IGameRenderer {
  init(canvas: HTMLCanvasElement, ctx: CanvasRenderingContext2D, dispatch?: ActionDispatcher): void;
  resize(width: number, height: number): void;
  render(state: any, ctx: CanvasRenderingContext2D): void;
  handlePointerDown(x: number, y: number): void;
  handlePointerMove(x: number, y: number): void;
  handlePointerUp(x: number, y: number): void;
  setDispatcher?(dispatch: ActionDispatcher): void;
  destroy(): void;
}
