# MANUAL_WEB_MITECHOY

## Guía Completa de Administración · v3.0 · Astro SSG

```
╔══════════════════════════════════════════════════════════════════╗
║   Mi Tec Hoy  ·  Sistema Estático  ·  Astro 4.x  ·  Tijuana BC  ║
║   Autor: Héctor I. Montaño  ·  pb.mitechoy.com  ·  Abril 2026   ║
╚══════════════════════════════════════════════════════════════════╝
```

> **Este manual es tu centro de operaciones.**
> Explica el *qué*, el *cómo* y el *por qué* de cada parte del sitio —
> desde editar una línea de texto hasta mantener el servidor vivo tras
> un reinicio de sistema.

-----

## ◈ ÍNDICE DE NAVEGACIÓN

|#                                   |Sección                  |Qué cubre                                               |
|------------------------------------|-------------------------|--------------------------------------------------------|
|[01](#01--arquitectura-del-proyecto)|Arquitectura del Proyecto|Estructura de carpetas, ecosistema Astro, flujo de build|
|[02](#02--gestión-de-contenido)     |Gestión de Contenido     |Blog, SERVICE_DATA, variables críticas, PocketBase      |
|[03](#03--diccionario-de-comandos)  |Diccionario de Comandos  |npm scripts, flujo dev → producción, deploy             |
|[04](#04--persistencia-del-servicio)|Persistencia del Servicio|systemd, PM2, auto-reinicio tras reboot                 |
|[05](#05--monitoreo-de-estado)      |Monitoreo de Estado      |Health checks, logs, scripts automáticos                |
|[06](#06--referencia-rápida)        |Referencia Rápida        |Checklist de deploy, bugs v3.0, mapa de secciones       |

-----

-----

## 01 · Arquitectura del Proyecto

```
● LED VERDE — Sección activa: ARQUITECTURA
```

### ¿Qué es MiTecHoy v3.0 exactamente?

MiTecHoy v3.0 es un **sitio estático generado con Astro 4.x (SSG — Static Site Generator)**.
Esto significa que Astro actúa como una herramienta de construcción: toma tus archivos fuente
(plantillas `.astro`, CSS, JavaScript) y los convierte en HTML puro antes de que el servidor
los entregue al visitante.

**¿Por qué importa esto?**
En producción no hay Node.js corriendo. No hay servidor de aplicaciones. No hay tiempo de
procesamiento por visita. El servidor web (Nginx) simplemente entrega archivos estáticos —
exactamente igual que si sirvieras una imagen. Esto lo hace extremadamente rápido, seguro
y barato de mantener.

```
  [ Navegador ]  →  petición HTTP  →  [ Nginx ]  →  lee dist/index.html  →  responde
                                                       ↑ un archivo estático, punto
```

PocketBase corre completamente separado en `pb.mitechoy.com` y solo se involucra
cuando el usuario envía un formulario. El sitio carga y funciona aunque PocketBase
esté caído.

-----

### Árbol de archivos comentado

```
mitechoy_v3/                    ← raíz del proyecto (donde corres npm)
│
├── public/                     ← TODO lo que pones aquí llega al navegador
│   │                             sin que Astro lo toque. URL directa: /js/main.js
│   ├── js/
│   │   └── main.js             ★ EL MÁS IMPORTANTE. Contiene:
│   │                               · SERVICE_DATA (equipos en reparación)
│   │                               · PB_URL (URL de PocketBase)
│   │                               · Toda la lógica interactiva del sitio
│   │
│   └── hector.jpeg             ← Foto de perfil del About.
│                                 DEBE existir antes de npm run build.
│                                 Si no está → el build falla silenciosamente.
│
├── src/                        ← Código fuente que Astro procesa y compila
│   │
│   ├── layouts/
│   │   └── Layout.astro        ← El "esqueleto" HTML de cada página.
│   │                             Contiene: <html>, <head>, Google Fonts,
│   │                             meta tags Open Graph, y el <script defer>
│   │                             que carga main.js. Editalo para cambiar
│   │                             el title, description o meta OG del sitio.
│   │
│   ├── pages/
│   │   └── index.astro         ← LA PÁGINA. Todo el HTML visible del sitio
│   │                             vive aquí. Astro convierte este archivo en
│   │                             /index.html dentro de dist/ al hacer build.
│   │                             Secciones organizadas con comentarios
│   │                             Ctrl+F → "SECTION: NOMBRE"
│   │
│   ├── styles/
│   │   └── global.css          ← TODO el CSS del sitio (~47 KB).
│   │                             Importado en index.astro. Astro lo extrae,
│   │                             optimiza y lo inyecta como stylesheet.
│   │                             Variables de color en :root, animaciones,
│   │                             componentes, responsive breakpoints.
│   │
│   └── components/             ← Carpeta vacía. Reservada para cuando
│                                 decidas dividir index.astro en componentes
│                                 reutilizables (NavBar.astro, Footer.astro…)
│
├── astro.config.mjs            ← Configuración de Astro.
│                                 · site: URL canónica del sitio (importante
│                                   para SEO y meta OG correctos)
│                                 · output: 'static' (default) → SSG puro
│
├── package.json                ← Dependencias y scripts npm.
│                                 Única dependencia: astro ^4.16.0
│
└── tsconfig.json               ← TypeScript config. Extiende astro/tsconfigs/base.
                                  Habilita el type-checking de archivos .astro.
```

-----

### El flujo de build explicado paso a paso

Cuando corres `npm run build`, esto es lo que pasa internamente:

```
PASO 1 · Astro lee astro.config.mjs
         → sabe que site = 'https://mitechoy.com'
         → sabe que output = 'static'

PASO 2 · Astro procesa src/pages/index.astro
         → resuelve el import de Layout.astro
         → resuelve el import de global.css
         → genera HTML estático completo

PASO 3 · Astro copia public/ → dist/
         → dist/js/main.js
         → dist/hector.jpeg
         (sin modificar, byte por byte)

PASO 4 · Astro escribe el resultado
         → dist/index.html  (el HTML compilado)
         → dist/_astro/     (CSS + chunks optimizados)

PASO 5 · Listo. dist/ es tu sitio completo.
         Sube ese directorio a cualquier servidor y funciona.
```

> **Regla de oro:** lo que editas en `src/` requiere rebuild.
> Lo que editas en `public/` solo necesita subirse al servidor.

-----

-----

## 02 · Gestión de Contenido

```
● LED VERDE — Sección activa: CONTENIDO
```

### 2.1 · El Blog

El blog en `#blog` es HTML estático dentro de `src/pages/index.astro`.
No hay base de datos, no hay API. Para agregar o modificar entradas,
editas el archivo directamente.

**¿Por qué así y no con un CMS?**
Porque para el volumen actual de publicaciones (menos de 10 entradas visibles),
un CMS añade complejidad sin beneficio real. Cuando el blog crezca a 20+ artículos,
el Apéndice B de esta guía explica cómo migrar a Astro Content Collections.

#### Navegar al blog en el archivo

Abre `src/pages/index.astro` y usa **Ctrl+F** para buscar:

```
SECTION: BLOG
```

Encontrarás dos zonas editables:

-----

#### Zona 1 — Artículo destacado (`.post-featured`)

Es el artículo grande con imagen completa que aparece a la izquierda.
Solo hay **uno**. Su estructura es:

```html
<article class="post-featured fade-up">

  <!-- IMAGEN: URL de Unsplash o ruta local como /mi-imagen.jpg -->
  <img class="post-featured-img"
       src="https://images.unsplash.com/photo-XXXXX?w=900&q=80"
       alt="Descripción accesible de la imagen"
       loading="lazy">

  <div class="post-featured-body">

    <!-- CATEGORÍA: texto libre, puedes poner varias con · -->
    <span class="post-cat">Arduino · Electrónica</span>

    <!-- TÍTULO: visible en la tarjeta -->
    <h3 class="post-featured-title">
      Sensor de temperatura WiFi con Arduino Nano ESP32
    </h3>

    <!-- DESCRIPCIÓN: 2-3 oraciones. No más. -->
    <p class="post-featured-desc">
      Construimos un sistema de monitoreo con alertas por Telegram...
    </p>

    <div class="post-meta">
      <span>📅 12 Mar 2025</span>   <!-- fecha -->
      <span>⏱ 8 min</span>          <!-- tiempo de lectura estimado -->
      <!-- ENLACE: puede ser YouTube, GitHub, o URL externa -->
      <a href="URL_DEL_ARTICULO" target="_blank" rel="noopener"
         class="read-more">Ver en YouTube →</a>
    </div>

  </div>
</article>
```

**Para actualizar el artículo destacado:** reemplaza los valores entre las etiquetas.
No elimines ni muevas las clases CSS — controlan el diseño visual.

-----

#### Zona 2 — Lista del sidebar (`.post-list-item`)

Son los artículos secundarios en la columna derecha. Puedes tener hasta 5-6
antes de que el scroll se vea raro. Están dentro del `<aside class="posts-sidebar">`.

**Estructura de cada artículo del sidebar:**

```html
<a href="URL_DEL_VIDEO_O_ARTICULO"
   target="_blank"
   rel="noopener"
   class="post-list-item">

  <!-- MINIATURA: recomendado 200x150px, Unsplash o local -->
  <img class="post-list-img"
       src="https://images.unsplash.com/photo-XXXXX?w=200&q=70"
       alt="Descripción breve"
       loading="lazy">

  <div class="post-list-info">
    <div class="post-list-cat">Categoría</div>
    <div class="post-list-title">Título del artículo aquí</div>
    <div class="post-list-date">DD Mes YYYY</div>
  </div>

</a>
```

**Para agregar un nuevo artículo:** copia el bloque completo de `<a>` a `</a>`,
pégalo antes del botón `sidebar-view-all`, y edita los valores.

**Para eliminar un artículo:** borra el bloque completo de `<a>` a `</a>`.

> ⚠️ **Recuerda:** después de cualquier cambio en `src/pages/index.astro`
> debes correr `npm run build` y subir el nuevo `dist/` al servidor.

-----

### 2.2 · El Panel de Servicios (SERVICE_DATA)

Este es el panel lateral que se abre desde el botón “◉ Servicios” en el footer.
Muestra los equipos en reparación con LED de estado en tiempo real.

**¿Dónde vive?** En `public/js/main.js`, sección `[6]`.
**¿Por qué en `public/` y no en `src/`?** Porque es JavaScript puro del cliente.
No necesita procesamiento de Astro. Al estar en `public/`, puedes actualizarlo
y subir solo ese archivo al servidor — sin necesidad de rebuild completo.

-----

#### Cómo editar SERVICE_DATA

Abre `public/js/main.js` y busca (**Ctrl+F**): `SERVICE_DATA`

```js
const SERVICE_DATA = [

  // ── EQUIPO ACTIVO ────────────────────────────────────────────
  {
    id:        "MT-2026-0021",    // Número de ticket visible al cliente
                                  // Formato: MT-AÑO-NÚMERO (ej. MT-2026-0001)

    device:    "Laptop Lenovo ThinkPad",  // Nombre del dispositivo
                                          // Sé específico: marca + modelo

    image:     "",                // Ruta a foto del equipo
                                  // "" → usa el ícono del STATUS_MAP
                                  // "img/lenovo.jpg" → foto en public/img/

    status:    "in_progress",     // Estado actual (ver tabla abajo)

    createdAt: "2026-03-28",      // Fecha de ingreso (YYYY-MM-DD)
                                  // Se muestra en el panel como "Ingresó: ..."

    completed: false              // false = aparece en "En servicio"
                                  // true  = se mueve a "Completados"
  },

  // ── AGREGA MÁS EQUIPOS COPIANDO EL BLOQUE ANTERIOR ──────────
  // {
  //   id: "MT-2026-0022",
  //   device: "iPhone 13 pantalla rota",
  //   image: "",
  //   status: "paused",
  //   createdAt: "2026-03-30",
  //   completed: false
  // },

];
```

-----

#### Tabla de estados disponibles

|Valor en `status`|LED en pantalla    |Etiqueta visible|Cuándo usarlo                                                                         |
|-----------------|-------------------|----------------|--------------------------------------------------------------------------------------|
|`"in_progress"`  |🟢 Verde parpadeante|En proceso      |Estás trabajando en el equipo ahora mismo                                             |
|`"paused"`       |🟡 Amarillo estático|En pausa        |Esperando pieza, pago adelantado o respuesta del cliente                              |
|`"stopped"`      |🔴 Rojo estático    |Detenido        |Trabajo suspendido por causa externa (cliente no responde, etc.)                      |
|`"completed"`    |🔵 Azul estático    |Completado      |Equipo listo. Cambiar también `completed: true` para moverlo a la sección de historial|

**Flujo típico de un equipo:**

```
Ingresa → in_progress → (pausa si falta pieza) → paused
                      → in_progress → completed
                                      ↓
                               completed: true
                         (desaparece de "activos", va a historial)
```

-----

#### Cambiar iconos o etiquetas del STATUS_MAP

Busca en `main.js`: `STATUS_MAP`

```js
const STATUS_MAP = {
  in_progress: { ledClass: "led-green",  label: "En proceso",  icon: "💻" },
  paused:      { ledClass: "led-yellow", label: "En pausa",    icon: "🖥️" },
  stopped:     { ledClass: "led-red",    label: "Detenido",    icon: "📺" },
  completed:   { ledClass: "led-blue",   label: "Completado",  icon: "✅" },
};
```

El `icon` se usa como placeholder cuando `image` está vacío (`""`).
Puedes cambiar el emoji libremente. El `label` es el texto que ve el cliente.
Las `ledClass` corresponden a clases CSS en `global.css` — no las cambies
a menos que también modifiques el CSS.

-----

### 2.3 · La URL de PocketBase (PB_URL)

PocketBase es el backend que recibe los formularios del sitio.
Su URL está definida en **una sola línea** de `main.js`:

```js
// public/js/main.js — línea 20
const PB_URL = "https://pb.mitechoy.com";
```

**¿Qué usa esta URL?**

```
PB_URL + "/api/collections/tickets/records"   ← Formulario de solicitud de servicio
PB_URL + "/api/collections/newsletter/records" ← Suscripción al newsletter
```

**¿Qué pasa si PocketBase está caído?** El sitio tiene fallback automático:
el formulario de tickets abre el cliente de correo del usuario con todos
los campos pre-llenados dirigidos a la dirección configurada. El visitante
nunca ve un error.

**Para cambiar la URL de PocketBase:** edita esa línea, guarda, sube el archivo
`public/js/main.js` al servidor. No se necesita rebuild de Astro.

-----

### 2.4 · Ticker de noticias (banda superior)

La banda de texto animado que corre en la parte superior del sitio.

**Dónde:** `src/pages/index.astro` → Ctrl+F: `SECTION: TICKER`

```html
<div class="ticker" aria-label="Novedades">
  <div class="ticker-track" id="ticker-track">
    <span>Nuevo curso de IA disponible en MiTecBlog</span>
    <span>Reparación de consolas en Tijuana · Xbox · PS5 · Switch</span>
    <!-- Agrega o quita <span> según necesites -->
    <!-- El contenido se duplica automáticamente por CSS para el loop -->
  </div>
</div>
```

**Para agregar un mensaje:** añade un `<span>Texto aquí</span>` dentro de `.ticker-track`.
**Para eliminar un mensaje:** borra el `<span>` correspondiente.

> ⚠️ Los `<span>` están duplicados en el HTML para que la animación CSS
> sea continua. Si agregas uno, agrégalo dos veces (en las dos mitades idénticas
> del ticker-track) para mantener la continuidad visual.

-----

### 2.5 · Sección About (perfil de Héctor)

**Dónde:** `src/pages/index.astro` → Ctrl+F: `SECTION: ABOUT`

La foto de perfil se carga desde:

```html
<img class="about-img" src="hector.jpeg" alt="Héctor I. Montaño" loading="lazy">
```

El archivo `hector.jpeg` debe estar en `public/`. Cuando Astro hace build,
lo copia a `dist/hector.jpeg` y el HTML generado lo referencia correctamente.

**Para cambiar la foto:**

1. Renombra tu nueva foto a `hector.jpeg`
1. Reemplaza `public/hector.jpeg` con el nuevo archivo
1. Corre `npm run build`
1. Sube el nuevo `dist/` al servidor

**Si quieres usar un nombre diferente para el archivo:**
Busca `src="hector.jpeg"` en `index.astro` y cámbialo por el nuevo nombre.
También deberás actualizar el build y el servidor.

-----

### 2.6 · Cards de servicios y tutorías

**Dónde:** `src/pages/index.astro` → Ctrl+F: `SECTION: SERVICIOS`

Los servicios están organizados en **tabs** (Tutorías / Reparaciones).
Cada card sigue esta estructura genérica:

```html
<div class="svc-card fade-up">
  <div class="svc-card-icon">🔧</div>      <!-- emoji o ícono -->
  <div class="svc-card-body">
    <h3 class="svc-card-title">Nombre del servicio</h3>
    <p class="svc-card-desc">Descripción breve en 1-2 líneas.</p>
  </div>
  <div class="svc-card-footer">
    <span class="svc-price">Desde $XXX MXN</span>
    <button class="btn-outline"
            onclick="openModal()">
      Solicitar →
    </button>
  </div>
</div>
```

-----

-----

## 03 · Diccionario de Comandos

```
● LED VERDE — Sección activa: COMANDOS
```

### 3.1 · Los 5 comandos que necesitas saber

Todos se corren desde la **raíz del proyecto** (la carpeta `mitechoy_v3/`
donde vive `package.json`). Nunca desde dentro de `src/` o `public/`.

-----

#### `npm install`

```bash
npm install
```

**¿Qué hace?** Lee `package.json` y descarga todas las dependencias
declaradas en `node_modules/`. En v3.0 la única dependencia es `astro ^4.16.0`.

**¿Cuándo correrlo?**

- La primera vez que clonas o descargas el proyecto
- Después de que alguien modifique `package.json`
- Si borras `node_modules/` por error

**¿Por qué tarda la primera vez?** Astro descarga su runtime completo (~150MB).
Las veces siguientes usa caché local y es instantáneo.

**¿Qué NO hacer?** No subas `node_modules/` al servidor ni al repositorio.
Está en `.gitignore` por una razón: ocupa cientos de MB y se regenera con `npm install`.

-----

#### `npm run dev`

```bash
npm run dev
# → Server rodando en http://localhost:4321/
```

**¿Qué hace?** Inicia el servidor de desarrollo de Astro con **Hot Module
Replacement (HMR)**. Cada vez que guardas un archivo en `src/`, el navegador
se actualiza automáticamente — sin necesidad de F5.

**¿Cuándo usarlo?** Mientras estás editando el sitio localmente.
Es tu herramienta de trabajo diario.

**¿Por qué el puerto 4321?** Astro elige ese puerto por defecto para no
chocar con otros servidores locales comunes (React usa 3000, Vue usa 5173, etc.)

**Lo que actualiza automáticamente (HMR activo):**

- Cambios en `src/pages/index.astro`
- Cambios en `src/styles/global.css`
- Cambios en `src/layouts/Layout.astro`

**Lo que NO actualiza automáticamente (requiere F5 manual):**

- Cambios en `public/js/main.js`
- Imágenes nuevas en `public/`
- Cambios en `astro.config.mjs`

> **Tip:** Si ves cambios en el CSS pero no en el JS, es porque `main.js`
> está en `public/` y el servidor dev lo sirve directamente sin procesamiento.
> Recarga el navegador con Ctrl+Shift+R (hard reload) para limpiar la caché.

-----

#### `npm run build`

```bash
npm run build
# → dist/ generado con HTML/CSS/JS optimizados
```

**¿Qué hace?** Compila todo el proyecto en modo producción y genera
el directorio `dist/` listo para subir al servidor.

**¿Cuándo correrlo?** Antes de cada deploy a producción. Es el paso
obligatorio entre “edité el código” y “lo subí al servidor”.

**¿Qué genera en `dist/`?**

```
dist/
├── index.html          ← Tu página, completamente renderizada
├── hector.jpeg         ← Copiada desde public/
├── js/
│   └── main.js         ← Copiado desde public/js/ (sin modificar)
└── _astro/
    └── index.HASH.css  ← CSS procesado y optimizado por Astro
```

**¿Por qué el CSS termina en `_astro/` con un hash?**
Astro genera un hash basado en el contenido del archivo. Si el CSS no cambia,
el hash es el mismo → el navegador usa su caché. Si cambias algo, el hash
es nuevo → el navegador descarga la versión fresca. Esto se llama
**cache busting** y evita que tus visitantes vean versiones obsoletas.

**¿Qué pasa si el build falla?**
Los errores más comunes son:

- `hector.jpeg` no existe en `public/`
- Sintaxis incorrecta en `index.astro`
- Error de TypeScript (corre `npm run check` antes para detectarlo)

-----

#### `npm run preview`

```bash
npm run preview
# → Preview del build en http://localhost:4321/
```

**¿Qué hace?** Sirve el contenido de `dist/` localmente, simulando
exactamente lo que verá un visitante en producción.

**¿En qué se diferencia de `npm run dev`?**

|                     |`npm run dev`               |`npm run preview`               |
|---------------------|----------------------------|--------------------------------|
|Sirve desde          |`src/` (compilando al vuelo)|`dist/` (archivos ya compilados)|
|HMR (auto-reload)    |✅ Sí                        |❌ No                            |
|Velocidad de arranque|Más lento                   |Instantáneo                     |
|Fiel a producción    |Aprox.                      |Exacto                          |
|Cuándo usarlo        |Durante desarrollo          |Antes de hacer deploy           |

**¿Cuándo usarlo?** Después del build, antes de subir al servidor.
Te permite confirmar que el sitio compilado se ve y funciona igual
que en desarrollo.

> **Tip de QA:** si algo funciona en `dev` pero no en `preview`, el problema
> está en cómo Astro compiló ese recurso. Revisa las rutas de imágenes
> y si algún asset está referenciado de forma absoluta vs. relativa.

-----

#### `npm run check`

```bash
npm run check
# → 0 errores = listo para build
```

**¿Qué hace?** Ejecuta el type-checker de Astro sobre todos tus archivos `.astro`.
Detecta errores de TypeScript, props faltantes en componentes y referencias a
variables que no existen.

**¿Cuándo correrlo?** Antes de un build importante, especialmente si agregaste
código nuevo a `index.astro` o modificaste `Layout.astro`.

-----

### 3.2 · Flujo completo: desde editar hasta producción

Este es el flujo de trabajo estándar para cualquier actualización del sitio:

```
┌─────────────────────────────────────────────────────────────────┐
│                    FLUJO DE TRABAJO ESTÁNDAR                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. EDITAR ──────────────────────────────────────────────────   │
│     Modifica src/pages/index.astro, src/styles/global.css,      │
│     o public/js/main.js según lo que necesites cambiar.         │
│                                                                  │
│  2. VERIFICAR EN DEV ─────────────────────────────────────────  │
│     $ npm run dev                                                │
│     Abre http://localhost:4321 y revisa que todo se vea bien.   │
│     Los cambios en src/ recargan solos. main.js requiere F5.    │
│                                                                  │
│  3. COMPILAR ─────────────────────────────────────────────────  │
│     $ npm run build                                              │
│     Verifica que no haya errores en la terminal.                │
│                                                                  │
│  4. VALIDAR EL BUILD ──────────────────────────────────────── ← │
│     $ npm run preview           ← no saltes este paso           │
│     Revisa el sitio una vez más desde los archivos compilados.  │
│                                                                  │
│  5. SUBIR AL SERVIDOR ────────────────────────────────────────  │
│     $ rsync -avz --delete dist/ usuario@servidor:/var/www/      │
│     — o el método que uses para tu hosting —                    │
│                                                                  │
│  6. VERIFICAR EN PRODUCCIÓN ──────────────────────────────────  │
│     $ curl -I https://mitechoy.com                              │
│     Debe retornar HTTP 200. Abre el sitio en el navegador.     │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

-----

### 3.3 · Comandos de deploy al servidor

**Con rsync (recomendado — sincroniza solo lo que cambió):**

```bash
# Sintaxis básica
rsync -avz --delete dist/ usuario@tu-servidor.com:/var/www/mitechoy.com/

# Desglose de flags:
#   -a  → archive mode: preserva permisos, timestamps, links simbólicos
#   -v  → verbose: muestra cada archivo que se transfiere
#   -z  → comprime durante la transferencia (útil en conexiones lentas)
#   --delete → elimina en el servidor archivos que ya no existen en dist/
#              MUY IMPORTANTE: sin esto, versiones viejas del build quedan
#              sirviendo y pueden romper el caché del navegador

# Ejemplo real con puerto SSH personalizado (si no usas el 22)
rsync -avz -e "ssh -p 2222" --delete dist/ usuario@servidor:/var/www/mitechoy.com/
```

**Con scp (simple, sin sincronización inteligente):**

```bash
# Sube todo el directorio dist/
scp -r dist/* usuario@servidor:/var/www/mitechoy.com/

# Sube solo main.js si solo actualizaste SERVICE_DATA
scp public/js/main.js usuario@servidor:/var/www/mitechoy.com/js/main.js
```

> **¿Por qué `--delete` es importante?**
> Cuando Astro genera el CSS con hash (`index.ABC123.css`), si después
> cambias el CSS el nuevo hash será distinto (`index.XYZ789.css`).
> Sin `--delete`, ambos archivos existen en el servidor. El HTML nuevo
> apuntará al nuevo hash, pero el viejo sigue ocupando espacio.
> Con `--delete`, el archivo obsoleto desaparece automáticamente.

-----

-----

## 04 · Persistencia del Servicio

```
● LED VERDE — Sección activa: PERSISTENCIA
```

### ¿Por qué necesitamos esto?

MiTecHoy v3.0 genera archivos estáticos — no hay un proceso Node.js activo.
El servidor que necesita persistencia es **Nginx** (que sirve los archivos de `dist/`)
y **PocketBase** (que recibe los formularios).

Cuando el servidor se reinicia (mantenimiento, corte de luz, actualización del OS),
estos servicios deben levantarse solos. Sin configuración explícita, habría que
iniciarlos manualmente cada vez.

-----

### 4.1 · Opción A — systemd (Producción · Recomendado)

systemd es el gestor de servicios nativo de Linux. Está presente en Ubuntu,
Debian, CentOS, Fedora y prácticamente cualquier VPS moderno. Es la opción
más robusta y la que debes usar en producción.

**Ventajas de systemd sobre otras opciones:**

- Integrado en el OS — sin dependencias extra
- Maneja dependencias entre servicios (`After=network.target`)
- Logs unificados en journald (`journalctl -u servicio`)
- Política de reinicio granular (`Restart=always`, `RestartSec=5`)
- Status en tiempo real con `systemctl status`

-----

#### Configurar Nginx para servir el sitio

```bash
# 1. Crear el archivo de configuración del sitio
sudo nano /etc/nginx/sites-available/mitechoy
```

Contenido del archivo:

```nginx
# /etc/nginx/sites-available/mitechoy

server {
    listen 80;
    listen [::]:80;
    server_name mitechoy.com www.mitechoy.com;

    # ← IMPORTANTE: apunta al directorio donde vive dist/
    # Asegúrate de que este path existe y que www-data tiene permisos de lectura
    root /var/www/mitechoy.com;
    index index.html;

    # Manejo de rutas: primero busca el archivo exacto,
    # luego como directorio, finalmente sirve index.html.
    # Esto asegura que rutas como /blog o /about sirvan index.html
    # en caso de que en el futuro Astro genere rutas dinámicas.
    location / {
        try_files $uri $uri/ /index.html;
    }

    # Caché agresiva para assets con hash (CSS, JS con fingerprint).
    # El navegador los guarda por 1 año — seguros porque Astro les
    # cambia el nombre al modificarse (cache busting automático).
    location ~* \.(js|css|woff2|woff|ttf|svg)$ {
        expires 1y;
        add_header Cache-Control "public, immutable";
        add_header Vary Accept-Encoding;
    }

    # Caché moderada para imágenes (30 días).
    location ~* \.(png|jpg|jpeg|gif|ico|webp)$ {
        expires 30d;
        add_header Cache-Control "public";
    }

    # Compresión gzip — reduce el tamaño de transferencia ~70%
    gzip on;
    gzip_vary on;
    gzip_min_length 1024;
    gzip_types
        text/css
        application/javascript
        application/json
        image/svg+xml
        text/html;
}
```

```bash
# 2. Activar el sitio (crear symlink en sites-enabled)
sudo ln -s /etc/nginx/sites-available/mitechoy /etc/nginx/sites-enabled/

# 3. Verificar que la configuración es válida
#    Debe decir: "syntax is ok" y "test is successful"
sudo nginx -t

# 4. Aplicar sin downtime (graceful reload, no restart)
sudo systemctl reload nginx

# 5. Verificar que Nginx arranca automáticamente con el sistema
sudo systemctl is-enabled nginx
# → "enabled" significa que sí. Si dice "disabled":
sudo systemctl enable nginx
```

-----

#### Crear servicio systemd para PocketBase

Si PocketBase corre en el mismo servidor que el sitio:

```bash
# 1. Crear el archivo de servicio
sudo nano /etc/systemd/system/pocketbase.service
```

```ini
# /etc/systemd/system/pocketbase.service

[Unit]
Description=PocketBase — Backend MiTecHoy
# El servicio espera a que la red esté disponible antes de arrancar.
# Crítico para PocketBase porque necesita puertos de red.
After=network.target
# Si falla repetidamente, systemd esperará antes de reintentar
StartLimitIntervalSec=0

[Service]
Type=simple
# Corre como www-data por seguridad (no como root)
User=www-data
Group=www-data
# Directorio de trabajo — donde vive el binario
WorkingDirectory=/opt/pocketbase
# Comando de arranque. Ajusta el puerto si PocketBase está detrás de un proxy
ExecStart=/opt/pocketbase/pocketbase serve \
    --http="127.0.0.1:8090" \
    --dir=/opt/pocketbase/pb_data

# Política de reinicio:
# always → se reinicia pase lo que pase (incluso si sale con código 0)
Restart=always
# Espera 5 segundos antes de reintentar (evita bucles de crash rápidos)
RestartSec=5

# Los logs van al journal del sistema (ver con journalctl -u pocketbase)
StandardOutput=journal
StandardError=journal

# Variables de entorno si las necesitas
# Environment="PB_ENCRYPTION_KEY=tu_clave_aqui"

[Install]
# Este servicio se habilita para el target "multi-user" (sistema operativo completo)
WantedBy=multi-user.target
```

```bash
# 2. Recargar systemd para que reconozca el nuevo servicio
sudo systemctl daemon-reload

# 3. Habilitar — se iniciará automáticamente en cada boot
sudo systemctl enable pocketbase

# 4. Iniciar ahora mismo
sudo systemctl start pocketbase

# 5. Verificar que está corriendo
sudo systemctl status pocketbase
# Deberías ver: Active: active (running) since ...
```

-----

### 4.2 · Opción B — PM2 (Staging / Preview Server)

PM2 es un gestor de procesos para Node.js. Úsalo cuando quieras servir
el sitio con `astro preview` (útil para staging o entornos de prueba
compartidos), pero no para producción.

**¿Por qué NO usar PM2 + astro preview en producción?**
Porque `astro preview` levanta un servidor Node.js para servir los archivos
estáticos. Nginx hace exactamente lo mismo sin consumir RAM de Node, sin
overhead de JavaScript y con mucha más eficiencia. Para archivos estáticos,
Nginx siempre gana.

**¿Para qué SÍ es útil PM2?**

- Tienes un VPS de staging donde quieres probar el sitio antes de subir a prod
- No tienes Nginx configurado todavía
- Quieres monitorear CPU/RAM del proceso de preview en tiempo real

```bash
# 1. Instalar PM2 globalmente
npm install -g pm2

# 2. Iniciar el servidor de preview como proceso PM2
#    --name: identificador del proceso en PM2
#    --cwd:  directorio donde correr el comando
pm2 start "npm run preview" \
    --name "mitechoy-preview" \
    --cwd /home/tuusuario/mitechoy_v3

# 3. Guardar la lista de procesos
#    Sin este paso, PM2 olvida los procesos al reiniciar
pm2 save

# 4. Configurar arranque automático con el sistema
pm2 startup
# → PM2 imprimirá un comando específico para tu OS. Cópialo y ejecútalo.
# Ejemplo del comando que PM2 puede mostrar:
# sudo env PATH=$PATH:/usr/bin pm2 startup systemd -u tuusuario --hp /home/tuusuario

# 5. Verificar
pm2 list
# Debería mostrar mitechoy-preview con status: online
```

**Comandos de operación de PM2:**

```bash
pm2 list                          # Ver todos los procesos
pm2 status                        # Estado resumido
pm2 logs mitechoy-preview         # Ver logs en tiempo real
pm2 logs mitechoy-preview --lines 50  # Ver últimas 50 líneas
pm2 restart mitechoy-preview      # Reiniciar
pm2 reload mitechoy-preview       # Reiniciar sin downtime (si aplica)
pm2 stop mitechoy-preview         # Detener sin eliminar
pm2 delete mitechoy-preview       # Eliminar de la lista de PM2
pm2 monit                         # Dashboard interactivo (CPU, RAM, logs)
```

-----

### 4.3 · Configurar HTTPS con Certbot (Let’s Encrypt)

Si el sitio aún no tiene HTTPS, este es el proceso estándar para un VPS con Nginx:

```bash
# 1. Instalar Certbot y el plugin de Nginx
sudo apt install certbot python3-certbot-nginx -y

# 2. Obtener certificado SSL (Certbot modifica nginx automáticamente)
sudo certbot --nginx -d mitechoy.com -d www.mitechoy.com

# 3. Certbot configura la renovación automática. Verifica que funcione:
sudo certbot renew --dry-run
# Debe decir "Congratulations, all simulated renewals succeeded"

# 4. Certbot instala un timer de systemd que renueva el certificado.
# Verifica que esté activo:
sudo systemctl status certbot.timer
```

-----

-----

## 05 · Monitoreo de Estado

```
● LED VERDE — Sección activa: MONITOREO
```

### 5.1 · Verificaciones rápidas desde terminal

Estos comandos te dan una foto del estado del sistema en segundos:

```bash
# ── SITIO WEB ─────────────────────────────────────────────────────

# HTTP status del sitio (esperar: 200)
curl -o /dev/null -s -w \
  "Status: %{http_code}  |  Tiempo: %{time_total}s  |  Tamaño: %{size_download} bytes\n" \
  https://mitechoy.com

# Verificar que el contenido correcto se está sirviendo
# (debe retornar un número > 0 si MiTecHoy está en el HTML)
curl -s https://mitechoy.com | grep -c "MiTecHoy"

# Cabeceras de respuesta completas
curl -I https://mitechoy.com


# ── POCKETBASE API ────────────────────────────────────────────────

# Health check del API (respuesta esperada: {"code":200,"message":"API is healthy."})
curl -s https://pb.mitechoy.com/api/health | python3 -m json.tool

# ── NGINX ─────────────────────────────────────────────────────────

sudo systemctl status nginx          # Estado general
sudo nginx -t                        # Validar configuración
sudo ss -tlnp | grep nginx           # Verificar que escucha en 80/443

# ── POCKETBASE SERVICIO ────────────────────────────────────────────

sudo systemctl status pocketbase     # Estado general
sudo ss -tlnp | grep pocketbase      # Verificar que escucha en su puerto
```

-----

### 5.2 · Leer logs en tiempo real

```bash
# Logs de acceso de Nginx (cada visita al sitio)
sudo tail -f /var/log/nginx/access.log

# Logs de error de Nginx (404s, 500s, problemas de configuración)
sudo tail -f /var/log/nginx/error.log

# Logs de PocketBase (vía journald)
sudo journalctl -u pocketbase -f

# Logs de las últimas 2 horas de PocketBase
sudo journalctl -u pocketbase --since "2 hours ago"

# Logs de Nginx de hoy con filtro de errores 5xx
sudo grep "\" 5" /var/log/nginx/access.log | tail -20
```

**¿Qué buscar en los logs de Nginx?**

```
# Una visita normal se ve así:
66.249.66.1 - - [01/Apr/2026:14:32:11 +0000] "GET / HTTP/1.1" 200 12453 "-" "Mozilla/5.0"
                                                                ↑
                                              200 = todo bien

# Un 404 (archivo no encontrado):
66.249.66.1 - - [01/Apr/2026:14:32:12 +0000] "GET /favicon.ico HTTP/1.1" 404 162

# Un 502 (PocketBase caído, si tienes proxy_pass configurado):
66.249.66.1 - - [01/Apr/2026:14:32:13 +0000] "POST /api/... HTTP/1.1" 502 579
```

-----

### 5.3 · Script de health check automático

Este script verifica el estado del sitio y registra cualquier problema.
Se puede ejecutar cada 5 minutos via cron.

```bash
# Crear el script
sudo nano /opt/scripts/mitechoy-healthcheck.sh
```

```bash
#!/bin/bash
# /opt/scripts/mitechoy-healthcheck.sh
# Health check automático para MiTecHoy v3.0
#
# Instalación:
#   chmod +x /opt/scripts/mitechoy-healthcheck.sh
#   crontab -e → añadir: */5 * * * * /opt/scripts/mitechoy-healthcheck.sh

SITIO="https://mitechoy.com"
POCKETBASE="https://pb.mitechoy.com/api/health"
LOG="/var/log/mitechoy-health.log"
ALERTA_EMAIL="hector@mitechoy.com"   # Opcional: para alertas por email

# ── Función de verificación ──────────────────────────────────────
check_url() {
    local url="$1"
    local nombre="$2"
    local esperado="${3:-200}"   # Código HTTP esperado (default: 200)

    local code
    code=$(curl -o /dev/null -s -w "%{http_code}" --max-time 15 "$url")

    if [ "$code" != "$esperado" ]; then
        local msg="[$(date '+%Y-%m-%d %H:%M:%S')] ❌ ERROR: $nombre respondió HTTP $code (esperado: $esperado)"
        echo "$msg" | tee -a "$LOG"

        # Enviar email de alerta (requiere mailutils instalado)
        # echo "$msg" | mail -s "🚨 Alerta MiTecHoy: $nombre caído" "$ALERTA_EMAIL"

        return 1
    else
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] ✅ OK: $nombre ($code)" >> "$LOG"
        return 0
    fi
}

# ── Verificaciones ───────────────────────────────────────────────
check_url "$SITIO"       "Sitio Web (mitechoy.com)"
check_url "$POCKETBASE"  "PocketBase API"

# ── Verificar servicios locales ──────────────────────────────────
for servicio in nginx pocketbase; do
    if ! systemctl is-active --quiet "$servicio"; then
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] ❌ SERVICIO CAÍDO: $servicio" | tee -a "$LOG"
        # Opcional: reiniciar automáticamente
        # sudo systemctl restart "$servicio" && echo "↩️ Reiniciado: $servicio" >> "$LOG"
    fi
done

# Rotar el log si supera 5MB
if [ -f "$LOG" ] && [ $(stat -c%s "$LOG") -gt 5242880 ]; then
    mv "$LOG" "${LOG}.bak"
    echo "Log rotado $(date)" > "$LOG"
fi
```

```bash
# Dar permisos de ejecución
chmod +x /opt/scripts/mitechoy-healthcheck.sh

# Instalar en cron (ejecutar cada 5 minutos)
crontab -e
```

Línea a agregar en crontab:

```cron
*/5 * * * * /opt/scripts/mitechoy-healthcheck.sh
```

-----

### 5.4 · Tabla de estados y acciones

|Servicio       |Verificación                                                 |Estado esperado|Acción si falla                    |
|---------------|-------------------------------------------------------------|---------------|-----------------------------------|
|Nginx          |`systemctl is-active nginx`                                  |`active`       |`sudo systemctl restart nginx`     |
|PocketBase     |`systemctl is-active pocketbase`                             |`active`       |`sudo systemctl restart pocketbase`|
|Sitio web      |`curl -s -o /dev/null -w "%{http_code}" https://mitechoy.com`|`200`          |Revisar Nginx error.log            |
|API PocketBase |`curl -s https://pb.mitechoy.com/api/health`                 |`{"code":200}` |Revisar `journalctl -u pocketbase` |
|Certificado SSL|`certbot certificates`                                       |Días > 30      |`sudo certbot renew`               |

-----

-----

## 06 · Referencia Rápida

```
● LED AZUL — Sección activa: REFERENCIA
```

### 6.1 · Checklist de deploy

Antes de cada deploy a producción, verifica esta lista:

```
ANTES DEL BUILD
─────────────────────────────────────────────────────────
[ ] public/hector.jpeg existe en la carpeta
    → ls -la public/hector.jpeg

[ ] PB_URL apunta a la URL correcta de PocketBase
    → Revisar línea 20 de public/js/main.js

[ ] SERVICE_DATA tiene los equipos actualizados
    → Revisar sección [6] de public/js/main.js

[ ] No hay errores de TypeScript/sintaxis
    → npm run check

DURANTE EL BUILD
─────────────────────────────────────────────────────────
[ ] El build completa sin errores
    → npm run build
    → Verificar que la terminal no muestra warnings críticos

[ ] dist/ fue generado con los archivos esperados
    → ls -la dist/
    → Confirmar que dist/index.html existe

ANTES DEL DEPLOY
─────────────────────────────────────────────────────────
[ ] El preview local se ve correcto
    → npm run preview
    → Abrir http://localhost:4321 y revisar visualmente

[ ] Los formularios funcionan (ticket + newsletter)
    → Probar el formulario de servicio en el preview local

DEPLOY
─────────────────────────────────────────────────────────
[ ] Archivos subidos con --delete para limpiar obsoletos
    → rsync -avz --delete dist/ usuario@servidor:/var/www/

POST-DEPLOY
─────────────────────────────────────────────────────────
[ ] Sitio responde HTTP 200 en producción
    → curl -I https://mitechoy.com

[ ] PocketBase API responde
    → curl -s https://pb.mitechoy.com/api/health

[ ] El sitio se ve correcto en el navegador
    → Abrir https://mitechoy.com en modo incógnito (sin caché)
```

-----

### 6.2 · Mapa de secciones del sitio

Todas las secciones del sitio están marcadas con comentarios en
`src/pages/index.astro`. Usa **Ctrl+F** con el texto exacto:

```
SECTION: TICKER     → Banda animada de noticias superior
SECTION: NAVBAR     → Menú de navegación con dropdowns
SECTION: HERO       → Banner principal (#inicio)
SECTION: BLOG       → Artículos y sidebar del blog (#blog)
SECTION: SERVICIOS  → Tabs de tutorías y reparaciones (#servicios)
SECTION: MITEC+     → MiTecTv y MiTecChat (#mitecplus)
SECTION: YOUTUBE    → Embed del canal YouTube (#youtube)
SECTION: NEWSLETTER → Formulario de suscripción (#newsletter)
SECTION: ABOUT      → Perfil de Héctor I. Montaño (#acerca)
SECTION: COMUNIDAD  → Grid de redes sociales y contactos (#links)
SECTION: FOOTER     → Pie de página

MODAL:SERVICIO      → Modal del formulario de ticket de servicio
MODAL:MATRIX        → Modal de bienvenida a MiTecChat
MODAL:OXXO          → Modal con datos de pago OXXO

SISTEMA:STATUS-TRIGGER → Botón flotante del panel de servicios
SISTEMA:STATUS-PANEL   → Panel lateral con equipos en reparación
```

-----

### 6.3 · Variables críticas en main.js

|Variable      |Línea aprox.|Qué controla                          |
|--------------|------------|--------------------------------------|
|`PB_URL`      |20          |URL del backend PocketBase            |
|`SERVICE_DATA`|183         |Array de equipos en reparación        |
|`STATUS_MAP`  |226         |Iconos, colores y etiquetas de estados|

-----

### 6.4 · Bugs corregidos en v3.0

|Bug               |Descripción                                                |Resolución                           |
|------------------|-----------------------------------------------------------|-------------------------------------|
|Archivo truncado  |El status panel HTML estaba cortado                        |Reconstruido completo desde cero     |
|Doble asignación  |`window.closeStatusPanel` y `closeStatusOutside` duplicados|Eliminados duplicados                |
|Script Cloudflare |Tag `cdn-cgi/scripts` generaba 404 fuera del proxy         |Removido del layout                  |
|Foto cortada      |`.about-img` tenía `max-height:420px`                      |Propiedad eliminada                  |
|Null crash OXXO   |`openOxxoModal()` sin null check                           |Añadido guard: `if (!el) return`     |
|`window.location=`|Asignación directa deprecada                               |Cambiada a `window.location.href=`   |
|`execCommand`     |API del portapapeles obsoleta                              |Reemplazada con `navigator.clipboard`|

-----

### 6.5 · Guía de colores y tipografía

Si necesitas mantener consistencia visual al agregar nuevo contenido o CSS:

**Paleta de colores (variables CSS en global.css)**

```css
/* Variables definidas en :root de global.css */
--teal:      #00c8b4;   /* Color principal de acento — botones, LEDs, highlights */
--teal-dark: #00a896;   /* Variante oscura para hover */
--dark:      #0a0c0f;   /* Fondo general del sitio */
--card:      #111318;   /* Fondo de cards y paneles */
--border:    #1e2229;   /* Bordes y separadores */
--text:      #c9d1d9;   /* Texto principal */
--muted:     #6e7681;   /* Texto secundario, fechas, metadatos */
```

**Tipografía**

```
Syne (400/600/700/800)  → Títulos, headings, nombre del logo
DM Mono (400/500)       → Código, labels técnicos, datos de ticket
Noto Serif JP (300/400) → Citas, detalles decorativos
```

-----

### 6.6 · Estructura de datos del formulario de ticket

Cuando un usuario envía el formulario de servicio, se crea este registro
en la colección `tickets` de PocketBase:

```json
{
  "nombre":        "Nombre completo del cliente",
  "telefono":      "664-XXX-XXXX",
  "email":         "cliente@email.com",
  "red_social":    "Instagram / Facebook / WhatsApp",
  "tipo_servicio": "Reparación de Laptop / Tutoría Python / ...",
  "descripcion":   "Descripción detallada del problema",
  "fecha_deseada": "2026-04-15",
  "modalidad":     "Presencial / En línea",
  "notas":         "Notas adicionales opcionales",
  "ticket_id":     "MTH-2026-4721"
}
```

El `ticket_id` se genera automáticamente en el cliente con:
`"MTH-" + new Date().getFullYear() + "-" + Math.floor(Math.random() * 9000 + 1000)`

-----

-----

```
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║   STACK TÉCNICO DE REFERENCIA                                 ║
║   ─────────────────────────────────────────────────────────  ║
║   Framework:   Astro 4.x · output: static (SSG)              ║
║   Estilos:     CSS nativo (global.css) · sin Tailwind         ║
║   JS:          Vanilla ES2020+ · sin frameworks en cliente    ║
║   Backend:     PocketBase · REST API                          ║
║   Servidor:    Nginx · archivos estáticos de dist/            ║
║   Node (dev):  v18 LTS o superior recomendado                 ║
║   SSL:         Let's Encrypt · Certbot                        ║
║   Tipografías: Syne · DM Mono · Noto Serif JP (Google Fonts)  ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
```

-----

*MiTecHoy — El Futuro con Nosotros · mitechoy.com · Tijuana, B.C.*