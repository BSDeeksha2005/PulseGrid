"use client"

import { useEffect, useMemo, useState } from "react"

import {
  Activity,
  Gauge,
  MapPin,
  Radio,
  Settings2,
  ShieldAlert,
  Truck,
} from "lucide-react"

type Severity = "High" | "Medium" | "Low"

type ApiAnomaly = {
  isAnomaly: boolean
  reason: string
  severity: Severity
  sourceEvent: {
    vehicleId: string
    locationId: string
    timestamp: string
    speed: number
    eventType: string
  }
}

type ApiSummary = {
  totalEventsProcessed: number
  totalAnomalies: number
  lowSeverityAnomalies: number
  mediumSeverityAnomalies: number
  highSeverityAnomalies: number
}

type Anomaly = {
  severity: Severity
  vehicleId: string
  locationId: string
  speed: number
  timestamp: string
  eventType: string
  reason: string
}

type MetricProps = {
  label: string
  value: string
  tone?: string
  suffix?: string
}

const API_BASE = "https://pulsegrid-f9k6.onrender.com/api"

const severityStyles: Record<Severity, string> = {
  High: "border-[#9d5148] bg-[#3a2523] text-[#e39a8f]",
  Medium: "border-[#9a7738] bg-[#382f20] text-[#d4b16e]",
  Low: "border-[#4f7668] bg-[#20312c] text-[#8fc0ab]",
}

function Metric({ label, value, tone, suffix }: MetricProps) {
  return (
    <div className="metric-cell">
      <div className="metric-label">
        <span className={`metric-dot ${tone}`} />
        {label}
      </div>

      <div className="flex items-baseline gap-2">
        <span className="telemetry-number">{value}</span>
        <span className="metric-suffix">{suffix}</span>
      </div>
    </div>
  )
}

function SeverityBar({
  label,
  value,
  total,
  color,
}: {
  label: string
  value: number
  total: number
  color: string
}) {
  const percentage = total > 0 ? (value / total) * 100 : 0

  return (
    <div className="severity-row">
      <span>{label}</span>

      <div className="severity-track">
        <div
          className={`h-full ${color}`}
          style={{ width: `${percentage}%` }}
        />
      </div>

      <strong>{value}</strong>
    </div>
  )
}

function formatTime(timestamp: string) {
  const date = new Date(timestamp)

  if (Number.isNaN(date.getTime())) {
    return timestamp
  }

  return date.toISOString().slice(11, 19)
}

function formatTimestamp(timestamp: string) {
  const date = new Date(timestamp)

  if (Number.isNaN(date.getTime())) {
    return timestamp
  }

  return `${date.toISOString().slice(11, 19)} UTC`
}

function mapApiAnomaly(item: ApiAnomaly): Anomaly {
  return {
    severity: item.severity,
    vehicleId: item.sourceEvent.vehicleId,
    locationId: item.sourceEvent.locationId,
    speed: item.sourceEvent.speed,
    timestamp: item.sourceEvent.timestamp,
    eventType: item.sourceEvent.eventType,
    reason: item.reason,
  }
}

export default function Page() {
  const [anomalies, setAnomalies] = useState<Anomaly[]>([])
  const [summary, setSummary] = useState<ApiSummary | null>(null)
  const [selected, setSelected] = useState<Anomaly | null>(null)
  const [filter, setFilter] = useState<Severity | "All">("All")
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    async function loadData() {
      try {
        setLoading(true)
        setError(null)

        const [summaryResponse, anomaliesResponse] = await Promise.all([
          fetch(`${API_BASE}/summary`),
          fetch(`${API_BASE}/anomalies`),
        ])

        if (!summaryResponse.ok || !anomaliesResponse.ok) {
          throw new Error("PulseGrid API request failed")
        }

        const summaryData: ApiSummary = await summaryResponse.json()
        const apiAnomalies: ApiAnomaly[] =
          await anomaliesResponse.json()

        const mappedAnomalies = apiAnomalies.map(mapApiAnomaly)

        setSummary(summaryData)
        setAnomalies(mappedAnomalies)
        setSelected(mappedAnomalies[0] ?? null)
      } catch (err) {
        console.error("PulseGrid API error:", err)

        setError(
          "Unable to connect to the PulseGrid API. Make sure the C++ server is running on port 8081."
        )
      } finally {
        setLoading(false)
      }
    }

    loadData()
  }, [])

  const filtered = useMemo(
    () =>
      filter === "All"
        ? anomalies
        : anomalies.filter((item) => item.severity === filter),
    [filter, anomalies]
  )

  const counts = {
    High: summary?.highSeverityAnomalies ?? 0,
    Medium: summary?.mediumSeverityAnomalies ?? 0,
    Low: summary?.lowSeverityAnomalies ?? 0,
  }

  const totalAnomalies = summary?.totalAnomalies ?? 0

  return (
    <main className="min-h-screen bg-[#111416] text-[#d9dfe0]">
      <header className="topbar">
        <div className="shell flex items-center justify-between">
          <div className="flex items-center gap-4">
            <div className="brand-mark" aria-hidden="true">
              <span />
              <span />
              <span />
            </div>

            <div>
              <div className="wordmark">PULSEGRID</div>
              <div className="eyebrow">
                TRAFFIC TELEMETRY / OPERATIONS
              </div>
            </div>
          </div>

          <div className="flex items-center gap-5">
            <div className="processing-state">
              <span className="status-dot" /> PROCESSING COMPLETE
            </div>

            <div className="header-divider" />

            <button
              aria-label="Settings"
              className="utility-button"
            >
              <Settings2 size={16} />
            </button>
          </div>
        </div>
      </header>

      <div className="shell py-8">
        <div className="mb-8 flex flex-wrap items-end justify-between gap-5">
          <div>
            <div className="eyebrow mb-3 flex items-center gap-2">
              <Activity size={13} />
              BATCH ANALYSIS / COMPLETED
            </div>

            <h1 className="page-title">Anomaly monitor</h1>

            <p className="page-subtitle">
              Detected anomalies from the completed traffic event analysis.
            </p>
          </div>

          <div className="run-stamp">
            <Radio size={13} /> PROCESSED EVENTS{" "}
            <span>{summary?.totalEventsProcessed ?? "—"}</span>
          </div>
        </div>

        {error ? (
          <div className="distribution-panel mb-6">
            <div className="eyebrow mb-2">
              PULSEGRID / API CONNECTION
            </div>

            <p className="page-subtitle">{error}</p>
          </div>
        ) : (
          <>
            <section
              aria-label="Operational overview"
              className="overview-grid"
            >
              <Metric
                label="Events processed"
                value={String(summary?.totalEventsProcessed ?? 0)}
                suffix="events"
                tone="green"
              />

              <Metric
                label="Total anomalies"
                value={String(totalAnomalies)}
                suffix="events"
                tone="amber"
              />

              <Metric
                label="High"
                value={String(counts.High).padStart(2, "0")}
                suffix="events"
                tone="red"
              />

              <Metric
                label="Medium"
                value={String(counts.Medium).padStart(2, "0")}
                suffix="events"
                tone="amber"
              />

              <Metric
                label="Low"
                value={String(counts.Low).padStart(2, "0")}
                suffix="events"
                tone="green"
              />
            </section>

            <section className="distribution-panel">
              <div>
                <div className="eyebrow mb-2">
                  ANOMALY DISTRIBUTION
                </div>

                <h2 className="section-title">
                  Classification profile
                </h2>
              </div>

              <div className="distribution-bar">
                <div
                  className="bg-[#a45349]"
                  style={{
                    width: `${
                      totalAnomalies > 0
                        ? (counts.High / totalAnomalies) * 100
                        : 0
                    }%`,
                  }}
                />

                <div
                  className="bg-[#a77f3d]"
                  style={{
                    width: `${
                      totalAnomalies > 0
                        ? (counts.Medium / totalAnomalies) * 100
                        : 0
                    }%`,
                  }}
                />

                <div
                  className="bg-[#56806f]"
                  style={{
                    width: `${
                      totalAnomalies > 0
                        ? (counts.Low / totalAnomalies) * 100
                        : 0
                    }%`,
                  }}
                />
              </div>

              <div className="distribution-legend">
                <SeverityBar
                  label="HIGH"
                  value={counts.High}
                  total={totalAnomalies}
                  color="bg-[#a45349]"
                />

                <SeverityBar
                  label="MEDIUM"
                  value={counts.Medium}
                  total={totalAnomalies}
                  color="bg-[#a77f3d]"
                />

                <SeverityBar
                  label="LOW"
                  value={counts.Low}
                  total={totalAnomalies}
                  color="bg-[#56806f]"
                />
              </div>
            </section>

            <section className="stream-layout">
              <div className="stream-panel">
                <div className="panel-header">
                  <div>
                    <div className="eyebrow mb-2 flex items-center gap-2">
                      <ShieldAlert size={13} />
                      ANOMALY EVENT STREAM
                    </div>

                    <h2 className="section-title">
                      Detected anomalies{" "}
                      <span className="count">
                        {filtered.length} SHOWN
                      </span>
                    </h2>
                  </div>

                  <div className="filter-group">
                    {(
                      ["All", "High", "Medium", "Low"] as const
                    ).map((item) => (
                      <button
                        key={item}
                        onClick={() => setFilter(item)}
                        className={
                          filter === item ? "active" : ""
                        }
                      >
                        {item.toUpperCase()}
                      </button>
                    ))}
                  </div>
                </div>

                <div className="table-wrap">
                  <table>
                    <thead>
                      <tr>
                        <th>Severity</th>
                        <th>Vehicle</th>
                        <th>Location</th>
                        <th>Event type</th>
                        <th>Reason</th>
                        <th>Speed</th>
                        <th className="text-right">Time</th>
                      </tr>
                    </thead>

                    <tbody>
                      {loading ? (
                        <tr>
                          <td colSpan={7}>
                            Loading telemetry...
                          </td>
                        </tr>
                      ) : (
                        filtered.map((item, index) => (
                          <tr
                            key={`${item.vehicleId}-${item.timestamp}-${index}`}
                            onClick={() => setSelected(item)}
                            className={
                              selected?.vehicleId ===
                                item.vehicleId &&
                              selected?.timestamp ===
                                item.timestamp
                                ? "selected"
                                : ""
                            }
                          >
                            <td>
                              <span
                                className={`severity-tag ${severityStyles[item.severity]}`}
                              >
                                {item.severity.toUpperCase()}
                              </span>
                            </td>

                            <td className="mono">
                              {item.vehicleId}
                            </td>

                            <td className="mono muted">
                              {item.locationId}
                            </td>

                            <td className="mono event-type">
                              {item.eventType}
                            </td>

                            <td>{item.reason}</td>

                            <td className="mono">
                              {item.speed.toFixed(1)} km/h
                            </td>

                            <td className="mono muted text-right">
                              {formatTime(item.timestamp)}
                            </td>
                          </tr>
                        ))
                      )}
                    </tbody>
                  </table>
                </div>
              </div>

              <aside
                className="inspector"
                aria-label="Selected anomaly"
              >
                <div className="panel-header inspector-header">
                  <div>
                    <div className="eyebrow mb-2">
                      SELECTED ANOMALY
                    </div>

                    <h2 className="section-title">
                      Inspector
                    </h2>
                  </div>

                  {selected && (
                    <span
                      className={`severity-tag ${severityStyles[selected.severity]}`}
                    >
                      {selected.severity.toUpperCase()}
                    </span>
                  )}
                </div>

                <div className="inspector-body">
                  {selected ? (
                    <>
                      <p className="reason">
                        {selected.reason}
                      </p>

                      <div className="detail-grid">
                        <div>
                          <span>VEHICLE ID</span>

                          <strong>
                            <Truck size={13} />
                            {selected.vehicleId}
                          </strong>
                        </div>

                        <div>
                          <span>LOCATION ID</span>

                          <strong>
                            <MapPin size={13} />
                            {selected.locationId}
                          </strong>
                        </div>

                        <div>
                          <span>EVENT TYPE</span>

                          <strong className="mono">
                            {selected.eventType}
                          </strong>
                        </div>

                        <div>
                          <span>SPEED</span>

                          <strong className="mono">
                            <Gauge size={13} />
                            {selected.speed.toFixed(1)} km/h
                          </strong>
                        </div>

                        <div className="col-span-2">
                          <span>OBSERVED</span>

                          <strong className="mono">
                            {formatTimestamp(
                              selected.timestamp
                            )}
                          </strong>
                        </div>
                      </div>
                    </>
                  ) : (
                    <p className="reason">
                      Waiting for telemetry data...
                    </p>
                  )}
                </div>
              </aside>
            </section>
          </>
        )}

        <footer className="footer">
          <span>PULSEGRID / COMPLETED PROCESSING RUN</span>
          <span>REST API SNAPSHOT · DATA AS SERVED</span>
        </footer>
      </div>
    </main>
  )
}
