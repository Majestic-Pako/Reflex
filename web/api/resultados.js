import { createClient } from '@supabase/supabase-js'

const TABLE_NAME = 'jugadores'
const MAX_NAME_LENGTH = 4

function sendJson(response, status, payload) {
    response.status(status).json(payload)
    }

    function readBody(body) {
    if (typeof body === 'string') {
        return JSON.parse(body)
    }

    return body ?? {}
    }

    function validateResult(payload) {
    const nombre =
        typeof payload.nombre === 'string'
        ? payload.nombre.trim().toUpperCase()
        : ''

    const puntaje = Number(payload.puntaje)
    const rondasAcertadas = Number(payload.rondas_acertadas)
    const rondaAlcanzada = Number(payload.ronda_alcanzada)

    if (!nombre || nombre.length > MAX_NAME_LENGTH) {
        return {
        error: `nombre debe tener entre 1 y ${MAX_NAME_LENGTH} caracteres`,
        }
    }

    const numericFields = [
        ['puntaje', puntaje],
        ['rondas_acertadas', rondasAcertadas],
        ['ronda_alcanzada', rondaAlcanzada],
    ]

    for (const [field, value] of numericFields) {
        if (!Number.isInteger(value) || value < 0) {
        return {
            error: `${field} debe ser un número entero mayor o igual a 0`,
        }
        }
    }

    return {
        data: {
        nombre,
        puntaje,
        rondas_acertadas: rondasAcertadas,
        ronda_alcanzada: rondaAlcanzada,
        },
    }
    }

    export default async function handler(request, response) {
    if (request.method !== 'POST') {
        response.setHeader('Allow', ['POST'])

        return sendJson(response, 405, {
        ok: false,
        error: 'Método no permitido. Usá POST.',
        })
    }

    const supabaseUrl =
        process.env.SUPABASE_URL ??
        process.env.VITE_SUPABASE_URL

    const supabaseKey =
        process.env.SUPABASE_SERVICE_ROLE_KEY ??
        process.env.VITE_SUPABASE_PUBLISHABLE_KEY

    if (!supabaseUrl || !supabaseKey) {
        return sendJson(response, 500, {
        ok: false,
        error: 'Faltan las variables de entorno de Supabase',
        })
    }

    let payload

    try {
        payload = readBody(request.body)
    } catch {
        return sendJson(response, 400, {
        ok: false,
        error: 'El cuerpo no contiene un JSON válido',
        })
    }

    const validation = validateResult(payload)

    if (validation.error) {
        return sendJson(response, 400, {
        ok: false,
        error: validation.error,
        })
    }

    const supabase = createClient(supabaseUrl, supabaseKey, {
        auth: {
        persistSession: false,
        autoRefreshToken: false,
        },
    })

    const { data, error } = await supabase
        .from(TABLE_NAME)
        .insert(validation.data)
        .select()
        .single()

    if (error) {
        console.error('Error al insertar resultado:', error)

        return sendJson(response, 500, {
        ok: false,
        error: 'No se pudo guardar el resultado',
        details: error.message,
        code: error.code,
        })
    }

    return sendJson(response, 201, {
        ok: true,
        message: 'Resultado guardado',
        result: data,
    })
}