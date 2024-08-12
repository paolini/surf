export type Vector = [number, number, number]

export function compute_area(a: Vector, b: Vector, c:Vector) {
    const [vx, vy, vz] = [b[0]-a[0], b[1]-a[1], b[2]-a[2]]
    const [wx, wy, wz] = [c[0]-a[0], c[1]-a[1], c[2]-a[2]]
    return 0.5*Math.sqrt((vy*wz-vz*wy)**2 + (vz*wx-vx*wz)**2 + (vx*wy-vy*wx)**2)
}

export function vector_diff(a: Vector, b: Vector): Vector {
    return [a[0]-b[0], a[1]-b[1], a[2]-b[2]]
}

export function vector_sum(a: Vector, b: Vector): Vector {
    return [a[0]+b[0], a[1]+b[1], a[2]+b[2]]
}

export function scale_vector(b: number, a: Vector): Vector {
    return [a[0]*b, a[1]*b, a[2]*b]
}

export function number_product(a: Vector, b: Vector): number {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]
}

export function squared_norm(a: Vector): number {
    return number_product(a,a)
}