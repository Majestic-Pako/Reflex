import { createClient } from '@supabase/supabase-js'

const supabaseUrl = import.meta.env.VITE_SUPABASE_URL
const supabasePublishableKey =
  import.meta.env.VITE_SUPABASE_PUBLISHABLE_KEY ??
  import.meta.env.VITE_SUPABASE_ANON_KEY

if (!supabaseUrl || !supabasePublishableKey) {
  throw new Error(
    'Faltan VITE_SUPABASE_URL y una clave publica de Supabase (VITE_SUPABASE_PUBLISHABLE_KEY o VITE_SUPABASE_ANON_KEY)'
  )
}

export const supabase = createClient(
  supabaseUrl,
  supabasePublishableKey
)
